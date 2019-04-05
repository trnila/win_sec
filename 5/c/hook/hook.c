#include <windows.h>
#include <stdio.h>

int main() {
	HANDLE dll = LoadLibrary("hook_dll.dll");
	if(!dll) {
		printf("could not find dll");
		return 1;
	}



	void* hook_handle = SetWindowsHookEx(WH_KEYBOARD, GetProcAddress(dll, "handler") , dll, 0);
	if(!hook_handle) {
		printf("could not install hook\n");
		return 1;
	}

	MSG msg;
	GetMessage(&msg, NULL, 0, 0);
	return 0;
}
