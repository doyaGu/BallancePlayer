param(
    [Parameter(Mandatory = $true)]
    [string]$Template,

    [Parameter(Mandatory = $true)]
    [string]$Output,

    [Parameter(Mandatory = $true)]
    [string]$VersionFile
)

$version = (Get-Content $VersionFile -Raw).Trim()
if ($version -notmatch '^(\d+)\.(\d+)\.(\d+)\.(\d+)$') {
    throw "VERSION must contain a numeric four-part version, got '$version'"
}

$encoding = [System.Text.Encoding]::GetEncoding(936)
$content = [System.IO.File]::ReadAllText($Template, $encoding)
$content = $content.Replace('@PROJECT_VERSION@', $version)
$content = $content.Replace('@PROJECT_VERSION_MAJOR@', $Matches[1])
$content = $content.Replace('@PROJECT_VERSION_MINOR@', $Matches[2])
$content = $content.Replace('@PROJECT_VERSION_PATCH@', $Matches[3])
$content = $content.Replace('@PROJECT_VERSION_TWEAK@', $Matches[4])

$outputDir = Split-Path -Parent $Output
if ($outputDir -and -not (Test-Path $outputDir)) {
    New-Item -ItemType Directory -Force -Path $outputDir | Out-Null
}

[System.IO.File]::WriteAllText($Output, $content, $encoding)
