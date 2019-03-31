function u() {
    $i=1;
    $ofs='';
    ("$($args[0]|%{[char[]]$_|%{$([char]([int]$_-$i))};' ';$i++})");
};