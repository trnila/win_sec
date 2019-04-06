#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include "../common.h"

DWORD load_with_check(HANDLE handle, void* remote_path) {
	printf("LoadLibraryA = %p\n", LoadLibrary);
	printf("GetLastError = %p\n", GetLastError);

	uint8_t prog[] = 
"\x49\x89\xCF\x48\x83\xEC\x28\x48\xB8\xB0\xEF\xCA\x2C\xFD\x7F\x00\x00\xFF\xD0\x48\x83\xF8\x00\x75\x16\x48\xB8\x60\x60\xCA\x2C\xFD\x7F\x00\x00\xFF\xD0\x49\x89\x07\x48\xC7\xC0\x00\x00\x00\x00\x48\x83\xC4\x28\xC3"

/*
mov r15, rcx                 ; backup pointer of dll_path (first arg)
sub rsp, 40                  ; grow stack 
mov rax, LoadLibraryA        ; LoadLibraryA address
call rax                     ; execute LoadLibraryA

cmp rax, 0                   ; does it failed?
jne end                      ; if not, jump to the end

mov rax, GetLastError        ; GetLastError address
call rax                     ; call GetLastError
mov [r15], rax               ; save code to the dll_path
mov rax, 0                   ; return 0 from this thread

end:
add rsp, 40                  ; decrement stack
ret                          ; end this thread

*/
;
	// place actual addresses to the instructions 
	*(uint64_t*) (prog + 9) = LoadLibraryA;
	*(uint64_t*) (prog + 0x21) = GetLastError;


	void *remote_prog = VirtualAllocEx(handle, NULL, sizeof(prog) , MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if(!remote_prog) {
		winerror("VirtualAlloc failed");
		return 1;
	}

	// write dll path to remote memory
	if(!WriteProcessMemory(handle, remote_prog, prog, sizeof(prog), NULL)) {
		winerror("WriteProcessMemory");
		return 1;
	}

	HANDLE t;
	if(!(t = CreateRemoteThread(handle, NULL, 0, remote_prog, remote_path, NULL, NULL))) {
		winerror("CreateRemoteThread");
		return 1;
	}
	WaitForSingleObject(t, INFINITE);
	DWORD dll_base;
	GetExitCodeThread(t, &dll_base);
	if(dll_base == 0) {
		// retrieve GetLastError from remote_path remote memory
		uint32_t err;
		if(!ReadProcessMemory(handle, remote_path, &err, sizeof(err), NULL)) {
			winerror("ReadProcessMemory(GetLastError)");
			exit(1);
		}
		char msg[128];
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
				msg, sizeof(msg), NULL);
		printf("failed to load dll: (%d) %s", err, msg);
		exit(1);
	}

	return dll_base;
}

DWORD load_simple(HANDLE handle, void* remote_path) {
	HANDLE t;
	// kernel32.dll functions are mapped for each process on the same address
	// so we can directly use LoadLibraryA pointer
	if(!(t = CreateRemoteThread(handle, NULL, 0, LoadLibraryA, remote_path, NULL, NULL))) {
		winerror("CreateRemoteThread");
		return 1;
	}

	// wait until LoadLibrary exists
	// exit code contains base address of loaded dll in remote process 
	WaitForSingleObject(t, INFINITE);
	VirtualFree(remote_path, 0, MEM_RELEASE);
	DWORD dll_base;
	GetExitCodeThread(t, &dll_base);
	return dll_base;
}

int main(int argc, char **argv) {
	if(argc <= 2) {
		fprintf(stderr, "Usage: %s process_name path/to.dll\n", argv[0]);
		return 1;
	}

	int pid = findproc(argv[1]);
	char *dll = argv[2];
	
	HANDLE handle = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, 0, pid);
	if(!handle) {
		winerror("OpenProcess failed: pid=%d", pid);
		return 1;
	}

	// inject our dll to remote process via remote thread, that starts executing LoadLibrary function
	// with path to that dll
	// because strings are passed as pointers, we have to allocate memory for path
	// and pass that memory as first argument to the LoadLibrary function
	// when thread exits, we get return value as EXIT CODE

	// allocate memory for dll path in remote process
	void *remote_path = VirtualAllocEx(handle, NULL, strlen(dll) , MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if(!remote_path) {
		winerror("VirtualAlloc failed");
		return 1;
	}

	// write dll path to remote memory
	if(!WriteProcessMemory(handle, remote_path, dll, strlen(dll), NULL)) {
		winerror("WriteProcessMemory");
		return 1;
	}
	HANDLE t;

	DWORD dll_base = load_with_check(handle, remote_path);
//	DWORD dll_base = load_simple(handle, remote_path);
	VirtualFree(remote_path, 0, MEM_RELEASE);

	printf("DLL base: %x\n", dll_base);
	if(dll_base == 0) {
		printf("Unable to load dll into the process");
		return 1;
	}

	// load DLL to our process so we can get offset address of function 'run'
	// attention: this executes code of DllMain in our process!
	HANDLE hh = LoadLibrary(dll);
	if(hh) {
		uint64_t base = GetProcAddress(hh, "run");
		if(base == NULL) {
			winerror("GetProcAddress(run)");
		} else {
			int offset = base - (long) hh;

			printf("run() offset: %x\n",  offset);

			// execute run function
			if(!(t = CreateRemoteThread(handle, NULL, 0, dll_base + offset, NULL, NULL, NULL))) {
				winerror("CreateRemoteThread");
				return 1;
			}
			WaitForSingleObject(t, INFINITE);
		}
	} else {
		winerror("LoadLibrary");
	}

	// Release dll from remote process
/*	if(!(t = CreateRemoteThread(handle, NULL, 0, FreeLibrary, dll_base, NULL, NULL))) {
		winerror("CreateRemoteThread");
		return 1;
	}
	WaitForSingleObject(t, INFINITE);
	DWORD status;
	GetExitCodeThread(t, &status);
	printf("return code: %x\n", status);*/

	return 0;
}
