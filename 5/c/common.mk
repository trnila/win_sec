CC=x86_64-w64-mingw32-gcc

all: $(TARGETS)

%.exe: %.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm $(TARGETS)
