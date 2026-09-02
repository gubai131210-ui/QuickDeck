# QuickDeck Architecture

Last reviewed: 2026-09-02 (post Phase 3 Iteration 4)

## Overview

QuickDeck is a Windows-first (V1) Qt 6 desktop app: application launcher + text clipboard history. One process, one SQLite database, one QML overlay window shared by two modes.

## CMake Targets

| Target | Type | Contents |
|--------|------|----------|
| `QuickDeckCore` | Static lib | All C++ except `main.cpp` |
| `QuickDeckLauncher` | QML module | Overlay + theme + components |
| `QuickDeck` | Executable | Entry, icons, i18n `.qm`, optional embedded `qt_zh_CN.qm` |

## Layer Map

```
main.cpp
  └── Application (composition root)
        ├── ApplicationContext (DI hub)
        │     ├── DatabaseManager → IAppRepository, IClipboardRepository, ISettingsStore
        │     ├── IPlatformServices (Win / Stub)
        │     ├── AppIndexer, ClipboardMonitor, SearchService
        ├── LauncherController + QML overlay
        ├── SettingsWindow, TrayManager, FirstRunWizard (Widgets)
        └── LocaleService
```

### Folder responsibilities

| Layer | Path | Owns |
|-------|------|------|
| **app** | `src/app/` | Lifecycle, `ApplicationContext` wiring |
| **core** | `src/core/` | Domain types, `Result<T>`, repository/platform interfaces, `UserMessages` error codes |
| **data** | `src/data/` | SQLite, migrations, repository adapters |
| **platform** | `src/platform/` | Hotkeys, launch, path open, auto-start (`win/`, `stub/`) |
| **services** | `src/services/` | Indexer, scanner, icon cache, clipboard monitor, search, locale, logger |
| **ui** | `src/ui/` | Launcher controller, models, widgets, QML |

## Key Seams (ADRs)

| Seam | Interface | Implementation | ADR |
|------|-----------|----------------|-----|
| Persistence | `IAppRepository`, `IClipboardRepository`, `ISettingsStore` | `data/repositories/sqlite_*` | [0003](adr/0003-repository-pattern.md) |
| Platform | `IPlatformServices` | `platform/win/*`, `platform/stub/*` | — |
| UI stack | Widgets vs QML | Settings/tray = Widgets; overlay = QML | [0002](adr/0002-mixed-widgets-qml-ui.md) |
| Storage | Migrations + FTS5 | `migrations/`, `MigrationRunner` | [0001](adr/0001-sqlite-local-storage.md) |

## Runtime Flow

### Startup

1. `ApplicationContext::initialize()` — open DB, run migrations, create platform + services
2. `LocaleService::apply_saved_language()` — load `quickdeck_*.qm` (+ optional Qt base zh)
3. Tray + Settings + QML overlay load; hotkeys register
4. First-run wizard if `setup.completed` is false
5. `AppIndexer::refresh_catalog()` (silent — no user toast)
6. `ClipboardMonitor::start()`

### User actions

| Action | Path |
|--------|------|
| `Alt+Space` | Hotkey → `LauncherController::show_search()` → QML visible |
| `Ctrl+Shift+V` | Hotkey → `show_clipboard()` |
| Type + Enter | `activate_selected()` → launch app / open path / re-copy clip |
| `Ctrl+Shift+P` | `toggle_pin_at()` — app or clipboard entry |
| Settings → Refresh index | `refresh_catalog(true)` → tray toast via `user_indexing_*` signals |

## i18n & Theme

- C++: `tr()` in Widgets/services; context = class name
- QML: `qsTr()`; context = **QML filename** (e.g. `LauncherOverlay`, `ResultRow`)
- Error codes: `core/user_messages.h` → `UserMessages::translate_error()` (context `UserMessages`)
- Visual tokens: `QuickDeckTheme` singleton — see [UI_THEME.md](UI_THEME.md), [I18N.md](I18N.md)

## Test Strategy

10 CTest executables (~30% of compilation units directly covered). Priority: migrations, controller state, ranking, scanner, error messages. Gaps: full indexer refresh, clipboard monitor runtime, platform launch, QML/widgets UI.

See [PROJECT_STATUS.md](PROJECT_STATUS.md) for the full matrix.

## Known Structural Debt (controlled, not blocking V1)

These are **accepted V1 shortcuts**. Fix when adding cross-platform or splitting CMake targets.

| Issue | Location | Recommended fix (post-V1) |
|-------|----------|---------------------------|
| Win TUs always in Core | `CMakeLists.txt` | Gate `win_platform_services` behind `WIN32` |
| Platform leak | `icon_cache.cpp`, `app_display.cpp` → `win_lnk_resolver` | Add `resolve_shortcut()` to `IPlatformServices` |
| Inverted dependency | `sqlite_app_repository` → `services/search_ranking` | Move ranking to `core/search_ranking` |
| Stub imports Win | `stub_platform_services` → `win_hotkey_mapper` | Extract shared hotkey parser to `core/` |
| Single static lib | All layers in `QuickDeckCore` | Optional: split `QuickDeckData` / `QuickDeckPlatform` targets |
| `ApplicationContext` hub | Concrete types in header | Factory or interface narrowing for tests |

## Dependency Rule (target state)

```
app → ui, services
ui → services, core (via ApplicationContext)
services → core, platform (via interfaces)
data → core only
platform → core only
```

Current violations are listed above; no new violations without ADR or explicit approval.
