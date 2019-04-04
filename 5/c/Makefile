CC=x86_64-w64-mingw32-gcc

all: simple.exe inject.exe test.dll gui.exe powershell.h

simple.exe: simple.c
	$(CC) $^ -o $@

inject.exe: inject.c
	$(CC) $^ -o $@

gui.exe: gui.cpp
	$(CC) $^ -o $@

test.dll: test.c
	$(CC) -shared $^ -o $@

powershell.h: ../../4/gen/1.ps1
	sed '1s/^\xFF\xFE//' $^ | base64 -w0 | sed -E 's/(.+)/const char script[] = "\1";/' > $@


clean:
	rm *.exe test.dll
