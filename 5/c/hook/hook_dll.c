#include <windows.h>
#include <stdio.h>
#include <process.h>

LRESULT __declspec(dllexport) CALLBACK handler(int nCode, WPARAM wParam, LPARAM lparam) {
	FILE *f = fopen("E:\\log", "a+");
	fprintf(f, "%d\n", _getpid());
	fclose(f);

	printf("CALLED!!");
	return CallNextHookEx(NULL, nCode, wParam, lparam);
}


BOOL __declspec(dllexport) DllMain(HINSTANCE hinstDLL, DWORD reason, LPVOID lpvReserved) {
	if (reason == DLL_PROCESS_ATTACH) {
		printf("attached to %d\n", GetCurrentProcessId());
	} else if(reason == DLL_PROCESS_DETACH) {
		printf("detached %d\n", GetCurrentProcessId());
	}
	return 1;
}
