#include <windows.h>
#include <stdio.h>

void fun() {
	int i = 0;
	for(;;) {
		printf("Attacker: %d\n", i++);
		Sleep(1000);
	}
}

void __declspec(dllexport) run() {
	CreateThread(NULL, 0, fun, NULL, NULL, NULL); 
}


BOOL __declspec(dllexport) DllMain(HINSTANCE hinstDLL, DWORD reason, LPVOID lpvReserved) {
	if (reason == DLL_PROCESS_ATTACH) {
		printf("attached to %d\n", GetCurrentProcessId());
	} else if(reason == DLL_PROCESS_DETACH) {
		printf("detached %d\n", GetCurrentProcessId());
	}
	return 1;
}
