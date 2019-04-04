#!/bin/sh
exe=../../victim.exe

get_fun() {
  echo $base + $base_code + $(objdump -t "$exe"  | grep $1'$' | grep -oE '0x[0-9a-f]{16}')
}

base=0x$(objdump -x "$exe"  | grep ^ImageBase | awk '{print $2}')
base_code=0x$(objdump -x "$exe"  | grep ^BaseOfCode | awk '{print $2}')

cat <<EOF | tee func.h
int (*remote_none)() = $(get_fun none);
int (*remote_num)() = $(get_fun num);
int (*remote_array)() = $(get_fun array);
int (*remote_add)() = $(get_fun add);
EOF
