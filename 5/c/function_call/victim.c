#include <windows.h>
#include <stdio.h>

int none() {
	printf("functionNone called\n");
	return 10;
}

int num(int val) {
	printf("functionNum called: %d\n", val);
	return 20;
}

int array(int *array) {
	printf("array called: %p %d %d\n", array, array[0], array[1]);
}

int add(int a, int b, int c) {
	printf("add %d %d %d\n", a, b, c);
	return a + b + c;
}

int main() {
	printf("none = %p\n", none);
	printf("%p\n", MessageBox);
	for(;;) {
		printf("Hello pid=%d\n", GetCurrentProcessId());
		Sleep(1000);
	}
	return 0;
}
