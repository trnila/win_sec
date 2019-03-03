all: simple.exe inject.dll

simple.exe: simple.c
	x86_64-w64-mingw32-gcc $^ -o $@

inject.dll: inject.c
	x86_64-w64-mingw32-gcc -shared $^ -o $@
