# QuickDeck

Cross-platform desktop launcher and clipboard history manager. **V1 targets Windows.**

## Features (V1)

- Application search and launch (Start Menu + Registry index)
- Clipboard text history with FTS5 search
- Global hotkeys: `Alt+Space` (launcher), `Ctrl+Shift+V` (clipboard)
- System tray with settings dialog
- Path open: type `C:\path\to\file` or `~/path`

## Won't Do (V1)

See [docs/V1_SCOPE.md](docs/V1_SCOPE.md) for the full out-of-scope list.

## Architecture

```
src/
├── app/        Application lifecycle, ApplicationContext
├── core/       Domain types, Result, repository interfaces
├── data/       SQLite, migrations, repository adapters
├── platform/   IPlatformServices (win/ stub/)
├── services/   Indexer, clipboard monitor, search
└── ui/         Widgets settings/tray + QML launcher overlay
```

- **QuickDeckCore** — static library (business logic)
- **QuickDeckLauncher** — QML module
- **QuickDeck** — executable

Read [AGENTS.md](AGENTS.md) and [CONTEXT.md](CONTEXT.md) before contributing.

## Database

- SQLite at `%APPDATA%/QuickDeck/data.db`
- Migrations in `migrations/NNN_*.sql` — never edit applied migrations; add new numbered files
- Tables: `apps`, `clipboard_entries`, `clipboard_fts`, `usage_stats`, `settings`

## Build

Requirements: Qt 6.5+, CMake 3.19+, C++17 compiler.

1. Open `CMakeLists.txt` in Qt Creator (Qt 6.11 MinGW kit)
2. Configure and build **QuickDeck**
3. Run from Qt Creator (migrations copied next to exe on build)

## Tests

```powershell
cd build
ctest
```

## License

MIT — see [LICENSE](LICENSE)

## Repository

GitHub: [gubai131210-ui/QuickDesk](https://github.com/gubai131210-ui/QuickDesk.git) (rename to QuickDeck planned)
