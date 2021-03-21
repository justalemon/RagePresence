$scripthookv_url = "http://www.dev-c.com/files/ScriptHookV_SDK_1.0.617.1a.zip"
$scripthookv_zip = "sdk\\scripthookv.zip"
$scripthookv_out = "sdk\\scripthookv"
$scripthookv_headers = @{
    "Referer" = "http://www.dev-c.com"
}

if (!(Test-Path $scripthookv_out))
{
    # Create the directory
    New-Item -Path "sdk" -Name "scripthookv" -ItemType "directory"
    
    # If the SDK ZIP doesn't exists, download it
    if (!(Test-Path $scripthookv_zip))
    {
        Invoke-WebRequest -Uri $scripthookv_url -OutFile $scripthookv_zip -MaximumRedirection 0 -Headers $scripthookv_headers
    }
    
    # Extract the files
    Expand-Archive -Path $scripthookv_zip -DestinationPath $scripthookv_out
    
    # Once everything is done, do some cleanup
    Remove-Item $scripthookv_zip -ErrorAction Ignore
    Remove-Item "$($scripthookv_out)\\inc\\nativeCaller.h" -ErrorAction Ignore
    Remove-Item "$($scripthookv_out)\\inc\\natives.h" -ErrorAction Ignore
}
