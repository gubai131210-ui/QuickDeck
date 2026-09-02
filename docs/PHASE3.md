# Phase 3 — App Indexer, Search, Launch + i18n/UI Framework

## Five-Agent Workflow (Iteration 2 — in progress)

| Agent | Role | Deliverables |
|-------|------|--------------|
| **1 Planner** | i18n gap analysis + scope | This doc, task breakdown, risk list |
| **2 i18n** | Fix translation coverage | TS context fix, missing strings, CMake QML lupdate |
| **3 Implementer** | Core Phase 3 features | LNK subtitle, pinned+recent, launch failure toast |
| **4 Reviewer** | Spec + standards check | ADR compliance, no scope creep |
| **5 Tester** | CRAP + reverse tests | Manual zh/en matrix, launch failure path |

### Anti-lazy rules (all agents)

- **禁止**只做占位 UI、不接线 i18n
- **禁止**硬编码颜色/字符串，必须用 Theme + `tr()`/`qsTr()`
- **禁止**跳过分语言切换验证（中/英各测一遍）
- **禁止**把设置页所有控件堆在一个 Tab（保持 General / Hotkeys / Clipboard）
- **禁止**未更新文档就宣称 Phase 完成
- **禁止** QML 翻译 context 写错（必须用 QML 文件名，如 `LauncherOverlay`）
- **禁止**只翻译设置页、不验证启动器 overlay / 托盘 / 对话框按钮

---

## i18n Gap Analysis (2026-09-02)

### Root cause: QML context mismatch

`qsTr()` in `LauncherOverlay.qml` resolves with context **`LauncherOverlay`** (filename).
TS files had context **`QObject`** → translator lookup failed → **all overlay strings stayed English**.

### Other gaps found

| Area | Issue | Status |
|------|-------|--------|
| Settings tab "Clipboard" | Missing in `SettingsWindow` TS context | Fixed |
| QMessageBox Yes/No | Qt standard buttons not in app TS | Added `QDialogButtonBox` entries |
| Tray menu | Had TS entries; updates via `language_changed` signal | OK |
| Settings widgets | Had TS entries; `changeEvent(LanguageChange)` | OK |
| First-run wizard | Has TS; only shown once at setup | OK |
| App names / paths in list | Data from OS, not translated | By design |
| Index refresh errors | Raw `Result` strings (English/SQLite) | Future: user-facing error codes |
| Launch failure message | Was silent | Fixed → tray toast |

### Verification checklist (user — Qt Creator)

1. Rebuild (CMake + TS changed — Run CMake first)
2. Settings → 简体中文 → Save
3. `Alt+Space` → overlay shows **搜索模式 / 应用 / 搜索应用或粘贴路径…**
4. `Ctrl+Shift+V` → **剪贴板模式 / 剪贴板 / 筛选剪贴板历史…**
5. Tray menu → **打开搜索 / 设置 / 退出**
6. Settings tabs → **常规 / 快捷键 / 剪贴板**
7. Hotkey conflict dialog → **是 / 否** buttons in Chinese

---

## Delivered in Iteration 1 (framework)

- [x] `LocaleService` + en/zh_CN translations embedded in exe
- [x] Settings language switch with live QML retranslate
- [x] Light glass overlay (`GlassPanel`, `SearchField`, `ResultRow`)
- [x] Empty-state messages + item count badge
- [x] `win_lnk_resolver` (Windows COM)
- [x] `LocaleServiceTest`
- [x] `QuickDeckTheme` singleton + Basic QML style fix

## Delivered in Iteration 2 (this pass)

- [x] Fix QML TS context (`LauncherOverlay`)
- [x] Missing zh strings (Clipboard tab, Yes/No, launch failure)
- [x] `.lnk` target shown in result subtitle (UNIQUE key unchanged)
- [x] Empty search: pinned apps first, then recent-by-usage
- [x] Launch failure tray notification
- [x] `qt_add_translations` includes QML files for future `lupdate`

## Remaining Phase 3 (Iteration 3+)

- [ ] App icon cache pipeline
- [ ] Pin toggle in overlay UI
- [ ] Index refresh success toast + translated error messages
- [ ] Indexer integration tests with fixture shortcuts
- [ ] Ship or bundle `qt_zh_CN.qm` fallback if MinGW Qt translations missing

## Manual Verification (user — Qt Creator)

1. Rebuild (CMake changed — Run CMake first)
2. Settings → Language → 简体中文 → Save → overlay strings 中文
3. Switch back to English → overlay English
4. `Alt+Space` — light glass panel, rounded corners, soft shadow
5. Empty catalog shows localized empty state
6. Refresh index in Settings — apps appear after scan
7. `.lnk` app row subtitle shows resolved `.exe` path
8. Launch invalid exe → tray shows localized failure toast

## Test Matrix

| Test | Covers |
|------|--------|
| LocaleServiceTest | language normalize/display |
| SearchRankingTest | search scoring |
| LauncherControllerTest | overlay controller |
| (manual) | i18n switch, glass UI, app launch, launch failure toast |
