#include <windows.h>
#include <stdio.h>


int main() {
//  int x = LoadLibrary("inject.dll");
  MSG msg;
  while(GetMessage(&msg, NULL, 0, 0) > 0) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  return 0;
}
