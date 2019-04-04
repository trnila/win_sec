TARGETS=inject.exe victim.exe func.h
CFLAGS="-Wl,--dynamicbase"
include ../common.mk

inject.exe: func.h

func.h: victim.exe
	./gen.sh


