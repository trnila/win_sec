#!/bin/sh
#pwsh generate.ps1

base64 remote.ps1 | ssh pi 'cat > /srv/http/trnila.eu/pvbs/icon.png'
