param(
    [string]$Configuration = "Release",
    [string]$MsvcBuildDir = "build-msvc2022-x86",
    [string]$OutputDir = "dist",
    [string]$PackageName = "",
    [switch]$SkipBuild,
    [switch]$IncludePdb
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
    param(
        [string[]]$CommandLine,
        [string]$WorkingDirectory = ""
    )

    Write-Host "> $($CommandLine -join ' ')"
    if ([string]::IsNullOrWhiteSpace($WorkingDirectory)) {
        & $CommandLine[0] @($CommandLine | Select-Object -Skip 1)
    } else {
        Push-Location $WorkingDirectory
        try {
            & $CommandLine[0] @($CommandLine | Select-Object -Skip 1)
        } finally {
            Pop-Location
        }
    }
    if ($LASTEXITCODE -ne 0) {
        throw "Command failed with exit code $LASTEXITCODE"
    }
}

function Build-MSVC2022 {
    param(
        [string]$RootDir,
        [string]$BuildPath,
        [string]$ConfigName
    )

    Invoke-Checked @("cmake", "-S", $RootDir, "-B", $BuildPath, "-G", "Visual Studio 17 2022", "-A", "Win32")
    Invoke-Checked @("cmake", "--build", $BuildPath, "--config", $ConfigName, "--target", "Player")
}

function Get-BinaryDir {
    param(
        [string]$BuildPath,
        [string]$ConfigName
    )

    $candidateDirs = @(
        (Join-Path (Join-Path $BuildPath "src") $ConfigName),
        (Join-Path $BuildPath "src")
    )

    foreach ($dir in $candidateDirs) {
        if (Test-Path (Join-Path $dir "Player.exe")) {
            return $dir
        }
    }

    throw "Could not find MSVC 2022 Player.exe under $BuildPath"
}

function New-PlayerPackage {
    param(
        [string]$RootDir,
        [string]$Version,
        [string]$ToolchainId,
        [string]$BinaryDir,
        [string]$OutputPath,
        [string]$ExplicitPackageName,
        [bool]$ShouldIncludePdb
    )

    if (-not [string]::IsNullOrWhiteSpace($ExplicitPackageName)) {
        $packageName = $ExplicitPackageName
    } else {
        $packageName = "BallancePlayer-$Version-$ToolchainId"
    }

    $stageRoot = Join-Path $OutputPath "_staging"
    $stagePath = Join-Path $stageRoot $packageName
    $zipPath = Join-Path $OutputPath "$packageName.zip"
    $shaPath = "$zipPath.sha256"

    $playerExe = Join-Path $BinaryDir "Player.exe"
    if (-not (Test-Path $playerExe)) {
        throw "Could not find Player.exe in $BinaryDir"
    }

    Remove-Item -LiteralPath $stagePath -Recurse -Force -ErrorAction SilentlyContinue
    New-Item -ItemType Directory -Path $stagePath -Force | Out-Null
    New-Item -ItemType Directory -Path $OutputPath -Force | Out-Null

    Copy-Item -LiteralPath $playerExe -Destination $stagePath

    foreach ($name in @("LICENSE", "README.md", "README_zh-CN.md")) {
        $path = Join-Path $RootDir $name
        if (Test-Path $path) {
            Copy-Item -LiteralPath $path -Destination $stagePath
        }
    }

    foreach ($name in @("CK2.dll", "VxMath.dll")) {
        $path = Join-Path $BinaryDir $name
        if (Test-Path $path) {
            Copy-Item -LiteralPath $path -Destination $stagePath
        }
    }

    if ($ShouldIncludePdb) {
        $pdb = Join-Path $BinaryDir "Player.pdb"
        if (Test-Path $pdb) {
            Copy-Item -LiteralPath $pdb -Destination $stagePath
        }
    }

    Remove-Item -LiteralPath $zipPath -Force -ErrorAction SilentlyContinue
    Remove-Item -LiteralPath $shaPath -Force -ErrorAction SilentlyContinue

    Compress-Archive -LiteralPath $stagePath -DestinationPath $zipPath -CompressionLevel Optimal -Force
    $hash = (Get-FileHash -Algorithm SHA256 -LiteralPath $zipPath).Hash.ToLowerInvariant()
    Set-Content -LiteralPath $shaPath -Value "$hash  $(Split-Path -Leaf $zipPath)" -Encoding ASCII

    Write-Host "Package: $zipPath"
    Write-Host "SHA256:  $shaPath"
}

$root = Split-Path -Parent $PSScriptRoot
$version = Get-ProjectVersion -RootDir $root
$msvcBuildPath = Resolve-PathUnderRoot -RootDir $root -PathValue $MsvcBuildDir
$outputPath = Resolve-PathUnderRoot -RootDir $root -PathValue $OutputDir

if (-not $SkipBuild) {
    Build-MSVC2022 -RootDir $root -BuildPath $msvcBuildPath -ConfigName $Configuration
}

$binaryDir = Get-BinaryDir -BuildPath $msvcBuildPath -ConfigName $Configuration
New-PlayerPackage -RootDir $root -Version $version -ToolchainId "msvc2022-x86" -BinaryDir $binaryDir `
    -OutputPath $outputPath -ExplicitPackageName $PackageName -ShouldIncludePdb:$IncludePdb
