#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include "../common.h"
#include "func.h"


void call_array(HANDLE handle) {
	// allocate array in remote process and call function 
	void *remote_array_mem = VirtualAllocEx(handle, NULL, sizeof(int) * 2, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if(!remote_array_mem) {
		winerror("VirtualAlloc");
		return 1;
	}

	const int nums[] = {22, 44};
	if(!WriteProcessMemory(handle, remote_array_mem, nums, sizeof(nums), NULL)) {
		winerror("WriteProcessMemory");
		return 1;
	}

	HANDLE t;
	if(!(t = CreateRemoteThread(handle, NULL, 0, remote_array, remote_array_mem, NULL, NULL))) {
		winerror("CreateRemoteThread");
		return 1;
	}

	WaitForSingleObject(t, INFINITE);
	VirtualFree(remote_array_mem, 0, MEM_RELEASE);
}

void inject_returncode(HANDLE handle) {
	// inject instruction that returns 123
	const uint8_t instructions[] = {
		0xB8, 0x7B, 0x00, 0x00, 0x00, // mov eax, 123
		0xc3  // ret
	};
	void *remote_mem = VirtualAllocEx(handle, NULL, sizeof(instructions), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if(!remote_mem) {
		winerror("VirtualAlloc");
		return 1;
	}
	if(!WriteProcessMemory(handle, remote_mem, instructions, sizeof(instructions), NULL)) {
		winerror("WriteProcessMemory");
		return 1;
	}
	
	HANDLE t;
	if(!(t = CreateRemoteThread(handle, NULL, 0, remote_mem, NULL, NULL, NULL))) {
		winerror("CreateRemoteThread");
		return 1;
	}

	WaitForSingleObject(t, INFINITE);
	DWORD status;
	GetExitCodeThread(t, &status);
	printf("return code: %d\n", status);
	VirtualFree(remote_mem, 0, MEM_RELEASE);
}

void inject_call_add(HANDLE handle) {
	union {
		uint8_t bytes[4];
		uint32_t addr;
	} fn;
	fn.addr = remote_add;

	const uint8_t instructions[] = {
		0xB9, 0x01, 0x00, 0x00, 0x00,                                          // mov ecx, 1
	       	0xBA, 0x02, 0x00, 0x00, 0x00,                                          // mov edx, 2
	       	0x41, 0xB8, 0x03, 0x00, 0x00, 0x00,                                    // mov r8d, 3
	       	0x48, 0xC7, 0xC0, fn.bytes[0], fn.bytes[1], fn.bytes[2], fn.bytes[3],  // mov rax, remote_add
	       	0xFF, 0xE0                                                             // jmp rax
	};
	void *remote_mem = VirtualAllocEx(handle, NULL, sizeof(instructions), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if(!remote_mem) {
		winerror("VirtualAlloc");
		return 1;
	}
	if(!WriteProcessMemory(handle, remote_mem, instructions, sizeof(instructions), NULL)) {
		winerror("WriteProcessMemory");
		return 1;
	}
	
	HANDLE t;
	if(!(t = CreateRemoteThread(handle, NULL, 0, remote_mem, NULL, NULL, NULL))) {
		winerror("CreateRemoteThread");
		return 1;
	}

	WaitForSingleObject(t, INFINITE);
	DWORD status;
	GetExitCodeThread(t, &status);
	printf("return code: %d\n", status);
	VirtualFree(remote_mem, 0, MEM_RELEASE);
}

int main(int argc, char **argv) {
	int pid = findproc("victim.exe");
	HANDLE handle = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, 0, pid);
	if(!handle) {
		printf("openProcess failed: %d %d\n", GetLastError(), pid);
		return 1;
	}

	if(!CreateRemoteThread(handle, NULL, 0, remote_none, NULL, NULL, NULL)) {
		winerror("CreateRemoteThread");
		return 1;
	}

	if(!CreateRemoteThread(handle, NULL, 0, remote_num, 42, NULL, NULL)) {
		winerror("CreateRemoteThread");
		return 1;
	}

	call_array(handle);
	inject_returncode(handle);
//	inject_call_add(handle);

	return 0;
}
