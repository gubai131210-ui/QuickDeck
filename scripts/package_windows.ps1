# Package QuickDeck for Windows distribution.
# Usage (from repo root or build dir):
#   .\scripts\package_windows.ps1 -BuildDir "D:\path\to\build\Release" -QtBinDir "D:\QT\6.11.1\mingw_64\bin"

param(
    [Parameter(Mandatory = $true)]
    [string]$BuildDir,

    [string]$QtBinDir = $env:QT_BIN_DIR,

    [string]$OutputDir = ""
)

$ErrorActionPreference = "Stop"

$BuildDir = (Resolve-Path $BuildDir).Path
$ExeName = "QuickDeck.exe"
$ExePath = Join-Path $BuildDir $ExeName

if (-not (Test-Path $ExePath)) {
    Write-Error "QuickDeck.exe not found in BuildDir: $BuildDir"
}

if ([string]::IsNullOrWhiteSpace($OutputDir)) {
    $OutputDir = Join-Path (Split-Path $BuildDir -Parent) "QuickDeck-dist"
}

$RepoRoot = Split-Path $PSScriptRoot -Parent
$QmlDir = Join-Path $RepoRoot "src\ui\qml"
$MigrationsSrc = Join-Path $RepoRoot "migrations"

function Find-WinDeployQt {
    param([string]$BinDir)
    if (-not [string]::IsNullOrWhiteSpace($BinDir)) {
        $candidate = Join-Path $BinDir "windeployqt.exe"
        if (Test-Path $candidate) { return $candidate }
    }
    $fromPath = Get-Command windeployqt.exe -ErrorAction SilentlyContinue
    if ($fromPath) { return $fromPath.Source }
    return $null
}

$WinDeployQt = Find-WinDeployQt -BinDir $QtBinDir
if (-not $WinDeployQt) {
    Write-Error "windeployqt.exe not found. Set -QtBinDir or QT_BIN_DIR."
}

if (Test-Path $OutputDir) {
    Remove-Item -Recurse -Force $OutputDir
}
New-Item -ItemType Directory -Path $OutputDir | Out-Null

Copy-Item $ExePath $OutputDir

Push-Location $OutputDir
try {
    Write-Host "Running windeployqt..."
    & $WinDeployQt --qmldir $QmlDir $ExeName
    if ($LASTEXITCODE -ne 0) {
        Write-Error "windeployqt failed with exit code $LASTEXITCODE"
    }
}
finally {
    Pop-Location
}

Copy-Item -Recurse $MigrationsSrc (Join-Path $OutputDir "migrations")

Write-Host ""
Write-Host "QuickDeck packaged successfully:" -ForegroundColor Green
Write-Host "  $OutputDir"
Write-Host ""
Write-Host "Smoke test: run QuickDeck.exe from that folder on a machine without Qt Creator."
