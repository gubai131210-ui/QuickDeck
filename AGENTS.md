# QuickDeck — Agent Guide

## Project

Cross-platform desktop launcher + clipboard history (**V1: Windows only**). Qt 6.5+, C++17, CMake.

**Status:** Phase 5 complete (2026-09-02). V1 feature-complete. See [docs/PROJECT_STATUS.md](docs/PROJECT_STATUS.md).

## Architecture

```
app/        → Application lifecycle, ApplicationContext
core/       → Domain types, Result, interfaces, UserMessages error codes
data/       → DatabaseManager, migrations, SQLite repository adapters
platform/   → IPlatformServices (win/ + stub/)
services/   → Indexer, scanner, icon cache, clipboard, search, locale, logger
ui/         → launcher_controller, models/, widgets/, qml/
```

**QuickDeckCore** static library holds everything except `main.cpp` and QML module registration.

Full layer diagram, runtime flows, and known debt: [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md).

## Domain Language

Read [CONTEXT.md](CONTEXT.md) before naming types, tests, or UI strings.

## Database

- Migrations in `migrations/NNN_name.sql`, applied in order by `MigrationRunner`
- Never edit applied migrations; add new numbered files
- FTS5 virtual table `clipboard_fts` syncs via triggers on `clipboard_entries`

## Coding Conventions

- `#pragma once`, member vars with trailing `_`, `CamelCase` types, `snake_case` functions
- Use `Result<T>` for fallible operations; no bare `new`/`delete`
- User-facing errors: stable codes in `core/user_messages.h` → translate via `UserMessages`
- UI strings: `tr()` in C++, `qsTr()` in QML — see [docs/I18N.md](docs/I18N.md)
- QML translation context = **filename** (e.g. `LauncherOverlay`, not `QObject`)
- Visual tokens: `QuickDeckTheme.qml` — see [docs/UI_THEME.md](docs/UI_THEME.md)
- Dependencies injected via `ApplicationContext`, not global singletons
- C++ style for non-trivial work: load [cpp-coding skill](~/.agents/skills/cpp-coding/SKILL.md)

## V1 Scope

See [docs/V1_SCOPE.md](docs/V1_SCOPE.md). Do not implement out-of-scope features without explicit request.

## Build & Test

- Open in Qt Creator with Qt 6.11 MinGW kit
- User tests locally (Chinese path — agent should not assume CLI build on user machine)
- After CMake/TS changes: user runs **Run CMake → Rebuild**
- Tests: 11 CTest targets — `ctest` from build directory
- Release: `cmake --build . --target QuickDeckPackage` — see [docs/RELEASE.md](docs/RELEASE.md)

## ADRs

[docs/adr/](docs/adr/) — consult before changing persistence, UI stack, or repository seams.

## Agent Skills

Skill selection map: [docs/AGENT_SKILLS.md](docs/AGENT_SKILLS.md).

## Phase Docs

| Doc | Content |
|-----|---------|
| [docs/PHASE2.md](docs/PHASE2.md) | Hotkeys, overlay UX, migrations |
| [docs/PHASE3.md](docs/PHASE3.md) | i18n, theme, indexer, pin, icon cache (complete) |
| [docs/PHASE4.md](docs/PHASE4.md) | Quick paste, retention toast (complete) |
| [docs/PHASE5.md](docs/PHASE5.md) | First-run polish, packaging (complete) |

## Known Debt (do not worsen)

- Direct `win_lnk_resolver` use in `icon_cache` / `app_display` (should go via platform interface)
- Win platform sources compiled unconditionally in CMake (V1 Windows-only OK)
