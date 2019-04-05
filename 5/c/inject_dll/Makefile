TARGETS=inject.exe powershell.dll powershell.h
include ../common.mk

powershell.h: ../../../4/gen/1.ps1
	sed '1s/^\xFF\xFE//' $^ | base64 -w0 | sed -E 's/(.+)/const char script[] = "\1";/' > $@

