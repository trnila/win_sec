#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include "../common.h"

void get_run_addr() {

}

int main(int argc, char **argv) {
	if(argc <= 2) {
		fprintf(stderr, "Usage: %s process_name path/to.dll\n", argv[0]);
		return 1;
	}

	int pid = findproc(argv[1]);
	char *dll = argv[2];
	
	HANDLE handle = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_WRITE, 0, pid);
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
	printf("DLL base: %x\n", dll_base);

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
	if(!(t = CreateRemoteThread(handle, NULL, 0, FreeLibrary, dll_base, NULL, NULL))) {
		winerror("CreateRemoteThread");
		return 1;
	}
	WaitForSingleObject(t, INFINITE);
	DWORD status;
	GetExitCodeThread(t, &status);
	printf("return code: %x\n", status);

	return 0;
}
