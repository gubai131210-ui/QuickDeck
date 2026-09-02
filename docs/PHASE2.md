# Phase 2 — Global Hotkeys & Overlay UX

## Delivered

- **Win hotkey mapper** — `QKeySequence` → Win32 `MOD_*` + `VK` with `MOD_NOREPEAT`
- **Global shortcuts** — `Alt+Space` (launcher), `Ctrl+Shift+V` (clipboard)
- **QML overlay** — fade/scale animation, Esc, ↑↓, Enter, blur-to-close with grace period
- **First-run wizard** — hotkey conflict detection, separate `QWizard` window
- **Settings** — hotkey validation (skip unchanged), reload on save
- **Migration runner** — BEGIN/END-aware SQL split; transactional apply; scoped queries
- **Database fixes** — safe destructor order; PRAGMA queries scoped; `list_recent` LEFT JOIN

## Tests (CRAP + reverse) — 6/6 passing

| Test | Seam protected | Reverse cases |
|------|----------------|---------------|
| WinHotkeyMapperTest | Hotkey parsing | empty, modifier-only, unsupported key |
| MigrationRunnerTest | Schema apply | missing dir, idempotent re-apply, bad SQL rollback |
| LauncherControllerTest | Show/dismiss/hide/wrap | isolated DB, reload_settings |
| SearchRankingTest | Ranking weights | prefix vs contains |
| PathResolverTest | Path detection | home resolve |
| ClipboardDeduplicationTest | Dedup query | empty content, seeded row |

## Manual verification (user — Qt Creator)

1. Build QuickDeck in Qt Creator (MinGW Qt 6.11.1)
2. First launch → wizard appears; cancel → wizard reappears on next launch
3. Complete wizard → `Alt+Space` opens search overlay with animation
4. Type, ↑↓ select, Enter launch, Esc close
5. Click outside → closes when blur setting enabled
6. Change hotkeys in Settings → works without restart

## Known follow-ups (Phase 3+)

- Tray notification when hotkey registration fails
- Clipboard `insert()` path: verify in runtime (FTS triggers); dedup query tested
