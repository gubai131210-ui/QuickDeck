# QuickDeck

Cross-platform desktop launcher and clipboard history manager. **V1 targets Windows.**

**Current status:** Phase 5 complete — V1 feature-complete. See [docs/PROJECT_STATUS.md](docs/PROJECT_STATUS.md).

## Features (V1)

- Application search and launch (Start Menu + Registry index)
- App icon cache, pin (`Ctrl+Shift+P`), usage-weighted search
- Clipboard text history with FTS5 search and pin
- Global hotkeys: `Alt+Space` (launcher), `Ctrl+Shift+V` (clipboard), `Ctrl+Alt+V` (quick paste)
- System tray with settings dialog (General / Hotkeys / Clipboard)
- First-run wizard: language + hotkey setup
- Path open: type `C:\path\to\file` or `~/path`
- Languages: English, 简体中文

## Won't Do (V1)

See [docs/V1_SCOPE.md](docs/V1_SCOPE.md) for the full out-of-scope list.

## Architecture

```
src/
├── app/        Application lifecycle, ApplicationContext
├── core/       Domain types, Result, interfaces, UserMessages, search_ranking
├── data/       SQLite, migrations, repository adapters
├── platform/   IPlatformServices (win/ + stub/)
├── services/   Indexer, scanner, icon cache, clipboard, search, locale
└── ui/         Widgets settings/tray + QML launcher overlay
```

- **QuickDeckCore** — static library (business logic)
- **QuickDeckLauncher** — QML module
- **QuickDeck** — executable

| Doc | Purpose |
|-----|---------|
| [AGENTS.md](AGENTS.md) | Agent conventions |
| [CONTEXT.md](CONTEXT.md) | Domain glossary |
| [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) | Layers, flows, debt |
| [docs/PROJECT_STATUS.md](docs/PROJECT_STATUS.md) | Phase checklist, tests |
| [docs/RELEASE.md](docs/RELEASE.md) | Windows packaging |
| [docs/AGENT_SKILLS.md](docs/AGENT_SKILLS.md) | Which skills to use |

## Database

- SQLite at `%APPDATA%/QuickDeck/data.db`
- Migrations in `migrations/NNN_*.sql` — never edit applied migrations; add new numbered files
- Tables: `apps`, `clipboard_entries`, `clipboard_fts`, `usage_stats`, `settings`

## Build

Requirements: Qt 6.5+, CMake 3.19+, C++17 compiler.

1. Open `CMakeLists.txt` in Qt Creator (Qt 6.11 MinGW kit)
2. **Run CMake** then build **QuickDeck**
3. Run from Qt Creator (migrations copied next to exe on build)

## Release (Windows)

```powershell
cmake --build . --target QuickDeckPackage
```

See [docs/RELEASE.md](docs/RELEASE.md) for details.

## Tests

11 automated tests. From build directory:

```powershell
ctest --output-on-failure
```

## License

MIT — see [LICENSE](LICENSE)

## Repository

GitHub: [gubai131210-ui/QuickDeck](https://github.com/gubai131210-ui/QuickDeck.git)
