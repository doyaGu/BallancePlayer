param(
    [ValidateSet("All", "VC6", "MSVC2022")]
    [string]$Toolchain = "All",
    [string]$Configuration = "Release",
    [string]$MsvcBuildDir = "build-msvc2022-x86",
    [string]$OutputDir = "dist",
    [string]$Tag = "",
    [string]$Vc6Root = "",
    [switch]$Draft,
    [switch]$Prerelease,
    [switch]$SkipPackage
)

$ErrorActionPreference = "Stop"
Set-StrictMode -Version 2.0

function Get-ProjectVersion {
    param([string]$RootDir)

    $cmakeLists = Join-Path $RootDir "CMakeLists.txt"
    $content = Get-Content $cmakeLists -Raw
    $match = [regex]::Match($content, 'project\s*\(\s*BallancePlayer[\s\S]*?VERSION\s+([0-9]+(?:\.[0-9]+)*)')
    if (-not $match.Success) {
        throw "Could not read project version from $cmakeLists"
    }
    return $match.Groups[1].Value
}

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

function Get-ToolchainIds {
    param([string]$RequestedToolchain)

    if ($RequestedToolchain -eq "All") {
        return @("vc6-x86", "msvc2022-x86")
    }
    if ($RequestedToolchain -eq "VC6") {
        return @("vc6-x86")
    }
    return @("msvc2022-x86")
}

$root = Split-Path -Parent $PSScriptRoot
$version = Get-ProjectVersion -RootDir $root
if ([string]::IsNullOrWhiteSpace($Tag)) {
    $Tag = "v$version"
}

$outputPath = Resolve-PathUnderRoot -RootDir $root -PathValue $OutputDir
$toolchainIds = Get-ToolchainIds -RequestedToolchain $Toolchain

if (-not $SkipPackage) {
    $packageScript = Join-Path $PSScriptRoot "package.ps1"
    Invoke-Checked @("powershell", "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", $packageScript,
        "-Toolchain", $Toolchain, "-Configuration", $Configuration, "-MsvcBuildDir", $MsvcBuildDir,
        "-OutputDir", $OutputDir, "-Vc6Root", $Vc6Root)
}

$assets = @()
foreach ($toolchainId in $toolchainIds) {
    $packageName = "BallancePlayer-$version-$toolchainId"
    $zipPath = Join-Path $outputPath "$packageName.zip"
    $shaPath = "$zipPath.sha256"
    if (-not (Test-Path $zipPath)) {
        throw "Package not found: $zipPath"
    }
    if (-not (Test-Path $shaPath)) {
        throw "Checksum not found: $shaPath"
    }
    $assets += $zipPath
    $assets += $shaPath
}

$gh = Get-Command gh -ErrorAction SilentlyContinue
if (-not $gh) {
    throw "GitHub CLI 'gh' was not found. Install it or run scripts\package.ps1 for local packaging only."
}

$packageLines = @()
foreach ($toolchainId in $toolchainIds) {
    if ($toolchainId -eq "vc6-x86") {
        $packageLines += "- BallancePlayer-$version-vc6-x86.zip: Visual C++ 6.0 x86 build"
    } else {
        $packageLines += "- BallancePlayer-$version-msvc2022-x86.zip: MSVC 2022 x86 build"
    }
}

$notesPath = Join-Path $outputPath "BallancePlayer-$version-release-notes.md"
@"
BallancePlayer $version

Attached packages:
$($packageLines -join "`n")

Each package contains:
- Player.exe
- ConfigTool.bat compatibility launcher for Player.exe --config-tool
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
