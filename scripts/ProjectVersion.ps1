function Get-VersionInfoFromFile {
    param([string]$VersionFile)

    if (-not (Test-Path $VersionFile)) {
        throw "VERSION file not found: $VersionFile"
    }

    $version = (Get-Content $VersionFile -Raw).Trim()
    $match = [regex]::Match($version, '^(\d+)\.(\d+)\.(\d+)\.(\d+)$')
    if (-not $match.Success) {
        throw "VERSION must contain a numeric four-part version, got '$version'"
    }

    return [PSCustomObject]@{
        Version = $version
        Major = $match.Groups[1].Value
        Minor = $match.Groups[2].Value
        Patch = $match.Groups[3].Value
        Tweak = $match.Groups[4].Value
    }
}

function Get-ProjectVersionInfo {
    param([string]$RootDir)

    return Get-VersionInfoFromFile -VersionFile (Join-Path $RootDir "VERSION")
}

function Get-ProjectVersion {
    param([string]$RootDir)

    return (Get-ProjectVersionInfo -RootDir $RootDir).Version
}
