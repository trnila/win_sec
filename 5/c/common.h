#pragma once

#include <tlhelp32.h>
#include <stdarg.h>

int findproc(const char*name) {
	int pid = -1;
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 process;
	process.dwSize = sizeof(process);
	if(Process32First(snapshot, &process)) {
		do {
			if(strcmp(process.szExeFile, name) == 0) {
				pid = process.th32ProcessID;
				break;
			}
		} while(Process32Next(snapshot, &process));
	}
	CloseHandle(snapshot);
	return pid;
}

void winerror(const char*fmt, ...) {
	int err = GetLastError();
	char msg[128];
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
			msg, sizeof(msg), NULL);

	va_list valist;
	va_start(valist, fmt);

	vfprintf(stderr, fmt, valist);
	fprintf(stderr, ": (%d) %s", err, msg);

	va_end(valist);
}
