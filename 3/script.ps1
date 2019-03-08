$disabled = Get-NetFirewallProfile -All | Where-Object -Property Enabled -EQ true | measure;
if ($disabled.Count -gt 0) {
  echo "will disable firewall";
  Start-Process powershell -WindowStyle Hidden -Verb runAs "Set-NetFirewallProfile -Profile Domain,Private,Public -Enabled false";
}
