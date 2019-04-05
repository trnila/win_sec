#include <windows.h>
#include <stdio.h>

int main() {
  LoadLibrary("inject_dll/powershell.dll");
  Sleep(INFINITE);
}
