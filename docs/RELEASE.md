# QuickDeck — Windows Release Packaging

V1 targets **Windows x64** with Qt 6.11 MinGW. User builds in Qt Creator; agent does not assume CLI build on Chinese-path machines.

## Prerequisites

- Successful **QuickDeck** build in Qt Creator (Release recommended)
- Qt `bin` on PATH, or note `windeployqt.exe` path (e.g. `D:\QT\6.11.1\mingw_64\bin`)
- MinGW runtime DLLs deployed by `windeployqt`

## Option A — CMake target (Qt Creator terminal)

From your **build directory**:

```powershell
cmake --build . --target QuickDeckPackage
```

Output folder: `QuickDeck-dist/` next to the build directory (see script for exact path).

## Option B — PowerShell script

```powershell
.\scripts\package_windows.ps1 `
  -BuildDir "D:\QT_CppPrograms\QuickDeck\build\Desktop_Qt_6_11_1_MinGW_64_bit-Release" `
  -QtBinDir "D:\QT\6.11.1\mingw_64\bin"
```

### Parameters

| Parameter | Required | Description |
|-----------|----------|-------------|
| `BuildDir` | Yes | Directory containing `QuickDeck.exe` |
| `QtBinDir` | No | Folder with `windeployqt.exe`; defaults to `$env:QT_BIN_DIR` or PATH |
| `OutputDir` | No | Default: `<BuildDir>/../QuickDeck-dist` |

## What the script does

1. Creates clean output directory
2. Copies `QuickDeck.exe`
3. Runs `windeployqt --qmldir <source>/src/ui/qml QuickDeck.exe`
4. Copies `migrations/` next to the exe
5. Prints path to distributable folder

## Smoke test (clean machine or VM)

1. Copy entire `QuickDeck-dist` folder
2. Run `QuickDeck.exe` — no Qt Creator required
3. Verify: tray icon, `Alt+Space`, `Ctrl+Shift+V`, `Ctrl+Alt+V`
4. First-run wizard on fresh `%APPDATA%/QuickDeck/data.db`

## Version

Check **Settings → About** or app property: `0.6.0` (Phase 5).

## Not included (V1)

- Code signing
- NSIS/MSI installer (future)
- Auto-update channel

See [PROJECT_STATUS.md](PROJECT_STATUS.md) for V1 checklist.
