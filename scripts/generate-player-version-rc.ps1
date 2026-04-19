param(
    [Parameter(Mandatory = $true)]
    [string]$Template,

    [Parameter(Mandatory = $true)]
    [string]$Output,

    [Parameter(Mandatory = $true)]
    [string]$VersionFile
)

$ErrorActionPreference = "Stop"
Set-StrictMode -Version 2.0

. "$PSScriptRoot\ProjectVersion.ps1"

$versionInfo = Get-VersionInfoFromFile -VersionFile $VersionFile

$encoding = [System.Text.Encoding]::GetEncoding(936)
$content = [System.IO.File]::ReadAllText($Template, $encoding)
$content = $content.Replace('@PROJECT_VERSION@', $versionInfo.Version)
$content = $content.Replace('@PROJECT_VERSION_MAJOR@', $versionInfo.Major)
$content = $content.Replace('@PROJECT_VERSION_MINOR@', $versionInfo.Minor)
$content = $content.Replace('@PROJECT_VERSION_PATCH@', $versionInfo.Patch)
$content = $content.Replace('@PROJECT_VERSION_TWEAK@', $versionInfo.Tweak)

$outputDir = Split-Path -Parent $Output
if ($outputDir -and -not (Test-Path $outputDir)) {
    New-Item -ItemType Directory -Force -Path $outputDir | Out-Null
}

[System.IO.File]::WriteAllText($Output, $content, $encoding)
