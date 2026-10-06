param(
    [string]$Configuration = "Release",
    [string]$MsvcBuildDir = "build-msvc2022-x86",
    [string]$OutputDir = "dist",
    [string]$Tag = "",
    [switch]$Draft,
    [switch]$Prerelease,
    [switch]$SkipPackage
)

$ErrorActionPreference = "Stop"
Set-StrictMode -Version 2.0

. "$PSScriptRoot\ProjectVersion.ps1"

function Resolve-PathUnderRoot {
    param(
        [string]$RootDir,
        [string]$PathValue
    )

    if ([System.IO.Path]::IsPathRooted($PathValue)) {
        return $PathValue
    }
    return (Join-Path $RootDir $PathValue)
}

function Invoke-Checked {
    param([string[]]$CommandLine)

    Write-Host "> $($CommandLine -join ' ')"
    & $CommandLine[0] @($CommandLine | Select-Object -Skip 1)
    if ($LASTEXITCODE -ne 0) {
        throw "Command failed with exit code $LASTEXITCODE"
    }
}

$root = Split-Path -Parent $PSScriptRoot
$version = Get-ProjectVersion -RootDir $root
if ([string]::IsNullOrWhiteSpace($Tag)) {
    $Tag = "v$version"
}

$outputPath = Resolve-PathUnderRoot -RootDir $root -PathValue $OutputDir

if (-not $SkipPackage) {
    $packageScript = Join-Path $PSScriptRoot "package.ps1"
    Invoke-Checked @("powershell", "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", $packageScript,
        "-Configuration", $Configuration, "-MsvcBuildDir", $MsvcBuildDir, "-OutputDir", $OutputDir)
}

$packageName = "BallancePlayer-$version-msvc2022-x86"
$zipPath = Join-Path $outputPath "$packageName.zip"
$shaPath = "$zipPath.sha256"
if (-not (Test-Path $zipPath)) {
    throw "Package not found: $zipPath"
}
if (-not (Test-Path $shaPath)) {
    throw "Checksum not found: $shaPath"
}
$assets = @($zipPath, $shaPath)

$gh = Get-Command gh -ErrorAction SilentlyContinue
if (-not $gh) {
    throw "GitHub CLI 'gh' was not found. Install it or run scripts\package.ps1 for local packaging only."
}

$notesPath = Join-Path $outputPath "BallancePlayer-$version-release-notes.md"
@"
BallancePlayer $version

Attached packages:
- $packageName.zip: MSVC 2022 x86 build

Each package contains:
- Player.exe
- README.md / README_zh-CN.md
- LICENSE

SHA256 checksum files are attached next to the packages.
"@ | Set-Content -LiteralPath $notesPath -Encoding UTF8

$releaseExists = $false
& gh release view $Tag *> $null
if ($LASTEXITCODE -eq 0) {
    $releaseExists = $true
}

if ($releaseExists) {
    Invoke-Checked (@("gh", "release", "upload", $Tag) + $assets + @("--clobber"))
} else {
    $args = @("gh", "release", "create", $Tag) + $assets + @("--title", "BallancePlayer $version", "--notes-file", $notesPath)
    if ($Draft) {
        $args += "--draft"
    }
    if ($Prerelease) {
        $args += "--prerelease"
    }
    Invoke-Checked $args
}

Write-Host "Release ready: $Tag"
