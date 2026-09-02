# QuickDeck — Agent Guide

## Project

Cross-platform desktop launcher + clipboard history (V1: Windows only). Qt 6.5+, C++17, CMake.

## Architecture

```
app/        → Application lifecycle, ApplicationContext
core/       → Domain types, Result, repository interfaces, IPlatformServices
data/       → DatabaseManager, migrations, SQLite repository adapters
platform/   → Platform-specific adapters (win/ for V1)
services/   → AppIndexer, ClipboardMonitor, SearchService, Logger
ui/         → widgets/ (tray, settings), qml/ (launcher overlay)
```

**QuickDeckCore** static library holds everything except `main.cpp` and QML registration glue.

## Domain Language

Read `CONTEXT.md` before naming types, tests, or UI strings.

## Database

- Migrations in `migrations/NNN_name.sql`, applied in order by `MigrationRunner`
- Never edit applied migrations; add new numbered files
- FTS5 virtual table `clipboard_fts` syncs via triggers on `clipboard_entries`

## Coding Conventions

- `#pragma once`, member vars with trailing `_`, `CamelCase` types, `snake_case` functions
- Use `Result<T>` for fallible operations; no bare `new`/`delete`
- UI strings: `tr()` in C++, `qsTr()` in QML
- Dependencies injected via `ApplicationContext`, not global singletons

## V1 Scope

See README.md "Won't Do" section. Do not implement out-of-scope features without explicit request.

## Build

Open in Qt Creator with Qt 6.11 MinGW kit. User tests locally (Chinese path constraints).

## ADRs

Architectural decisions in `docs/adr/`. Consult before changing persistence, UI stack, or module seams.
