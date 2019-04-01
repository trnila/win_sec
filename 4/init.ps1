echo "Stopping antivirus";
Stop-Process -Name mspaint -ErrorAction SilentlyContinue;

$stopPath="$env:APPDATA\signedin";
if(Test-Path "$stopPath") {
    echo terminating
    exit
};
echo "Continuing"