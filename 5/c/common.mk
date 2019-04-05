CC=x86_64-w64-mingw32-gcc
CPP=x86_64-w64-mingw32-g++

all: $(TARGETS)

%.exe: %.c
	$(CC) $(CFLAGS) $^ -o $@

%.exe: %.cpp
	$(CPP) $(CFLAGS) $^ -o $@

%.dll: %.c
	$(CC) $(CFLAGS) -shared $^ -o $@

clean:
	rm $(TARGETS)
