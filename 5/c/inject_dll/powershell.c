#include <windows.h>
#include <stdio.h>
#include "powershell.h"

void __declspec(dllexport) run() {
	char arguments[sizeof(script) + 128];
	snprintf(arguments, sizeof(arguments), "powershell.exe -EncodedCommand \"%s\"", script);

	STARTUPINFO info={sizeof(info)};
	PROCESS_INFORMATION processInfo;
	CreateProcessA("C:\\Windows\\System32\\WindowsPowershell\\v1.0\\powershell.exe", arguments, NULL, NULL, TRUE, 0, NULL, NULL, &info, &processInfo);
	WaitForSingleObject(processInfo.hProcess, INFINITE);
}


BOOL __declspec(dllexport) DllMain(HINSTANCE hinstDLL, DWORD reason, LPVOID lpvReserved) {
	if (reason == DLL_PROCESS_ATTACH) {
		printf("attached to %d\n", GetCurrentProcessId());
	} else if(reason == DLL_PROCESS_DETACH) {
		printf("detached %d\n", GetCurrentProcessId());
	}
	return 1;
}
