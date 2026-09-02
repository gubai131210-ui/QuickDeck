# QuickDeck Project Status

Last updated: 2026-09-02

## Version

- App version string: `0.4.0` (in `Application::initialize`)
- V1 target: Windows only
- Qt kit: 6.11 MinGW (user local build)

## Phase Completion

| Phase | Focus | Status |
|-------|-------|--------|
| **1** | Scaffold, DB, migrations, core services | Done |
| **2** | Hotkeys, QML overlay, tray, settings, migration runner | Done — see [PHASE2.md](PHASE2.md) |
| **3** | i18n, theme, indexer polish, icon cache, pin, error codes | Done (Iterations 1–4) — see [PHASE3.md](PHASE3.md) |
| **4** | Clipboard monitor UX polish, simulated paste, retention UI | Not started |
| **5** | First-run polish, performance, release packaging | Not started |

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
| System tray + 3-tab settings | Done |
| First-run wizard | Done |
| en / zh_CN i18n | Done |
| Mint Sky glass overlay UI | Done |
| User-facing error messages | Done (codes in `UserMessages`) |
| Simulated paste (`Ctrl+Alt+V` etc.) | **Not wired** — platform stub exists |
| macOS / Linux | Out of scope V1 |

## Automated Tests (10/10)

| Test | Covers |
|------|--------|
| MigrationRunnerTest | SQL split, transactions, rollback |
| SearchRankingTest | Match + usage + recency scoring |
| PathResolverTest | Path detection and resolve |
| WinHotkeyMapperTest | QKeySequence → Win32 modifiers/VK |
| LauncherControllerTest | Overlay state, app + clipboard pin |
| ClipboardDeduplicationTest | Dedup query |
| LocaleServiceTest | Language normalize/display |
| AppScannerTest | `.lnk` root scan |
| IconCacheTest | PNG icon cache (Windows) |
| UserMessagesTest | Error code translation |

Run: configure in Qt Creator, then `ctest` from build dir.

## Manual Verification (user)

User builds in Qt Creator (Chinese path — agent does not run production builds on user machine).

1. `Alt+Space` / `Ctrl+Shift+V` — overlay modes
2. Settings → Language → 简体中文 — overlay + tray + settings Chinese
3. Settings → Refresh index — tray toast (including 0 apps message)
4. Pin with `Ctrl+Shift+P` in both modes
5. Launch app — icon in list after index refresh

## Repository

- GitHub: [gubai131210-ui/QuickDeck](https://github.com/gubai131210-ui/QuickDeck.git)
- Legacy remote name `QuickDesk` may redirect — prefer QuickDeck naming in new docs/commits

## Next Recommended Work

1. Wire simulated paste hotkey (V1 platform code exists)
2. Clipboard retention enforcement UI feedback
3. Move `search_ranking` to `core/` (debt cleanup)
4. Release packaging (windeployqt, installer)
5. Optional: repository integration tests with in-memory SQLite
