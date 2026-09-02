# QuickDeck V1 Scope

## In Scope

- Windows application indexer (Start Menu `.lnk` + Registry uninstall keys)
- Text clipboard monitoring, deduplication, FTS5 search
- Single QML overlay with Search / Clipboard modes
- Global hotkeys, system tray, 3-tab settings
- Path open without file index

## Out of Scope (V1)

1. File name index and search
2. Custom commands, calculator, web search, system commands
3. Image / HTML / file-path clipboard entries
4. Tags, merge, edit, favorites, templates
5. Plugin system
6. macOS / Linux builds
7. Export / import / encrypted backup
8. Pinyin and fuzzy matching
9. `Ctrl+Alt+V` quick-paste hotkey
10. Translation, TTS, QR codes
11. Portable mode and custom DB path UI

## Performance Targets (V1)

| Metric | Target |
|--------|--------|
| Overlay invoke | < 200 ms |
| Search (1k items) | < 100 ms |
| Idle memory | < 120 MB |

## Development Phases

1. Skeleton + DB + migrations
2. Platform hotkeys + QML shell + tray
3. App indexer + search + launch
4. Clipboard monitor + FTS UI
5. Settings + first-run + polish
