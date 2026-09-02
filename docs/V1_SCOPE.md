# QuickDeck V1 Scope

## In Scope

- Windows application indexer (Start Menu `.lnk` + Registry uninstall keys)
- Text clipboard monitoring, deduplication, FTS5 search
- Single QML overlay with Search / Clipboard modes
- Global hotkeys, system tray, 3-tab settings
- Path open without file index
- Quick paste hotkey (`Ctrl+Alt+V` default)
- First-run wizard with language and hotkey setup
- Windows release packaging (`windeployqt` script)

## Out of Scope (V1)

1. File name index and search
2. Custom commands, calculator, web search, system commands
3. Image / HTML / file-path clipboard entries
4. Tags, merge, edit, favorites, templates
5. Plugin system
6. macOS / Linux builds
7. Export / import / encrypted backup
8. Pinyin and fuzzy matching
9. Translation, TTS, QR codes
10. Portable mode and custom DB path UI
11. Code signing and MSI installer

## Performance Targets (V1)

| Metric | Target |
|--------|--------|
| Overlay invoke | < 200 ms |
| Search (1k items) | < 100 ms |
| Idle memory | < 120 MB |
| Startup | Tray + hotkeys before background index completes |

## Development Phases

1. Skeleton + DB + migrations — **Done**
2. Platform hotkeys + QML shell + tray — **Done** ([PHASE2.md](PHASE2.md))
3. App indexer + search + launch + i18n/theme — **Done** ([PHASE3.md](PHASE3.md))
4. Clipboard UX polish + simulated paste — **Done** ([PHASE4.md](PHASE4.md))
5. First-run polish + release packaging — **Done** ([PHASE5.md](PHASE5.md))

See [PROJECT_STATUS.md](PROJECT_STATUS.md) for feature-level checklist.
