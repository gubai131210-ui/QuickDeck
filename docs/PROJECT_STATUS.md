# QuickDeck Project Status

Last updated: 2026-09-02

## Version

- App version string: `0.7.0` (in `Application::initialize`)
- V1 target: Windows only
- Qt kit: 6.11 MinGW (user local build)

## Phase Completion

| Phase | Focus | Status |
|-------|-------|--------|
| **1** | Scaffold, DB, migrations, core services | Done |
| **2** | Hotkeys, QML overlay, tray, settings, migration runner | Done — see [PHASE2.md](PHASE2.md) |
| **3** | i18n, theme, indexer polish, icon cache, pin, error codes | Done — see [PHASE3.md](PHASE3.md) |
| **4** | Quick paste, simulate paste, retention toast, search_ranking→core | Done — see [PHASE4.md](PHASE4.md) |
| **5** | First-run polish, deferred indexing, release packaging | Done — see [PHASE5.md](PHASE5.md) |
| **6** | Dual-page UX, command palette, QML settings, V1 debt | Done — see [PHASE6.md](PHASE6.md) |

## V1 Feature Checklist

| Feature | Status |
|---------|--------|
| App index (Start Menu + Registry) | Done |
| App search + usage-weighted ranking | Done |
| App launch + `.lnk` support | Done |
| Path open in Search Mode | Done |
| Icon cache on index refresh | Done |
| Pin app / clipboard (`Ctrl+Shift+P`) | Done |
| Text clipboard monitor + dedup | Done |
| Clipboard FTS search | Done |
| Global hotkeys | Done |
| Command palette (`>` builtins) | Done |
| Dual-page overlay (Search / Clipboard tabs) | Done |
| QML Settings overlay | Done |
| Ctrl+Enter simulate paste (clipboard) | Done |
| Delete clipboard entry | Done |
| Debug log + log rotation | Done |
| en / zh_CN i18n | Done |
| Theme dark / follow system | Deferred |
| macOS / Linux | Out of scope V1 |

## Automated Tests (12/12)

| Test | Covers |
|------|--------|
| CommandRouterTest | `>` command matching |
| MigrationRunnerTest | SQL split, transactions, rollback |
| SearchRankingTest | Match + usage + recency scoring |
| PathResolverTest | Path detection and resolve |
| WinHotkeyMapperTest | QKeySequence → Win32 modifiers/VK |
| LauncherControllerTest | Overlay state, pin, quick paste |
| ClipboardDeduplicationTest | Dedup query |
| ClipboardRetentionTest | Retention trim + removed count |
| LocaleServiceTest | Language normalize/display |
| AppScannerTest | `.lnk` root scan |
| IconCacheTest | PNG icon cache (Windows) |
| UserMessagesTest | Error code translation (incl. paste) |

Run: configure in Qt Creator, then `ctest` from build dir.

## Manual Verification (user)

1. `Alt+Space` → tab switch Search/Clipboard; `>` commands
2. `Ctrl+Shift+V` → clipboard page; Enter / Ctrl+Enter / Delete
3. `>settings` or tray Settings → QML settings panel
4. Pin with `Ctrl+Shift+P` in both modes
5. `Ctrl+Alt+V` quick paste

## Repository

- GitHub: [gubai131210-ui/QuickDeck](https://github.com/gubai131210-ui/QuickDeck.git)

## Next Recommended Work

1. Theme dark + follow system
2. First-run wizard QML migration
3. Pinyin search / file index (V1.2+)
