param(
    [ValidateSet("All", "VC6", "MSVC2022")]
    [string]$Toolchain = "All",
    [string]$Configuration = "Release",
    [string]$MsvcBuildDir = "build-msvc2022-x86",
    [string]$OutputDir = "dist",
    [string]$PackageName = "",
    [string]$Vc6Root = "",
    [switch]$SkipBuild,
    [switch]$IncludePdb
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

function Build-VC6 {
    param(
        [string]$RootDir,
        [string]$Vc6RootPath,
        [string]$ConfigName
    )

    $effectiveVc6Root = $Vc6RootPath
    if ([string]::IsNullOrWhiteSpace($effectiveVc6Root)) {
        $effectiveVc6Root = $env:VC6_ROOT
    }

    $args = @("/nologo", "/f", "Makefile", "CFG=$ConfigName")
    if (-not [string]::IsNullOrWhiteSpace($effectiveVc6Root)) {
        $nmake = Join-Path $effectiveVc6Root "Bin\nmake.exe"
        if (-not (Test-Path $nmake)) {
            throw "VC6 nmake.exe not found: $nmake"
        }
        $args += "VC6_ROOT=$effectiveVc6Root"
    } else {
        $nmakeCommand = Get-Command nmake.exe -ErrorAction SilentlyContinue
        if (-not $nmakeCommand) {
            throw "VC6 nmake.exe not found. Set -Vc6Root, set VC6_ROOT, or run from a VC6 command prompt."
        }
        $nmake = $nmakeCommand.Source
    }

    Invoke-Checked (@($nmake) + $args) -WorkingDirectory $RootDir
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
        [string]$RootDir,
        [string]$ToolchainId,
        [string]$BuildPath,
        [string]$ConfigName
    )

    if ($ToolchainId -eq "vc6-x86") {
        $dir = Join-Path $RootDir "Bin"
        if (Test-Path (Join-Path $dir "Player.exe")) {
            return $dir
        }
        throw "Could not find VC6 Player.exe in $dir"
    }

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
    $configToolBat = Join-Path $BinaryDir "ConfigTool.bat"
    if (-not (Test-Path $playerExe)) {
        throw "Could not find Player.exe in $BinaryDir"
    }
    if (-not (Test-Path $configToolBat)) {
        throw "Could not find ConfigTool.bat next to Player.exe in $BinaryDir"
    }

    Remove-Item -LiteralPath $stagePath -Recurse -Force -ErrorAction SilentlyContinue
    New-Item -ItemType Directory -Path $stagePath -Force | Out-Null
    New-Item -ItemType Directory -Path $OutputPath -Force | Out-Null

    Copy-Item -LiteralPath $playerExe -Destination $stagePath
    Copy-Item -LiteralPath $configToolBat -Destination $stagePath

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

    if (Test-Path (Join-Path $stagePath "ConfigTool.exe")) {
        throw "Package staging unexpectedly contains ConfigTool.exe"
    }

    Remove-Item -LiteralPath $zipPath -Force -ErrorAction SilentlyContinue
    Remove-Item -LiteralPath $shaPath -Force -ErrorAction SilentlyContinue

    Compress-Archive -LiteralPath $stagePath -DestinationPath $zipPath -CompressionLevel Optimal
    $hash = (Get-FileHash -Algorithm SHA256 -LiteralPath $zipPath).Hash.ToLowerInvariant()
    Set-Content -LiteralPath $shaPath -Value "$hash  $(Split-Path -Leaf $zipPath)" -Encoding ASCII

    Write-Host "Package: $zipPath"
    Write-Host "SHA256:  $shaPath"
}

$root = Split-Path -Parent $PSScriptRoot
$version = Get-ProjectVersion -RootDir $root
$msvcBuildPath = Resolve-PathUnderRoot -RootDir $root -PathValue $MsvcBuildDir
$outputPath = Resolve-PathUnderRoot -RootDir $root -PathValue $OutputDir
$toolchainIds = Get-ToolchainIds -RequestedToolchain $Toolchain

if (-not [string]::IsNullOrWhiteSpace($PackageName) -and $toolchainIds.Count -ne 1) {
    throw "-PackageName can only be used when packaging one toolchain."
}

foreach ($toolchainId in $toolchainIds) {
    if (-not $SkipBuild) {
        if ($toolchainId -eq "vc6-x86") {
            Build-VC6 -RootDir $root -Vc6RootPath $Vc6Root -ConfigName $Configuration
        } else {
            Build-MSVC2022 -RootDir $root -BuildPath $msvcBuildPath -ConfigName $Configuration
        }
    }

    $binaryDir = Get-BinaryDir -RootDir $root -ToolchainId $toolchainId -BuildPath $msvcBuildPath -ConfigName $Configuration
    New-PlayerPackage -RootDir $root -Version $version -ToolchainId $toolchainId -BinaryDir $binaryDir `
        -OutputPath $outputPath -ExplicitPackageName $PackageName -ShouldIncludePdb:$IncludePdb
}
