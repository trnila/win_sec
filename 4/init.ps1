Stop-Process -Name AudioPlayer -ErrorAction SilentlyContinue;

$stopPath="$env:APPDATA\signedin";
if(Test-Path "$stopPath") {
    echo terminating
    exit
};