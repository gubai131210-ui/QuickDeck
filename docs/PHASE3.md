# Phase 3 — App Indexer, Search, Launch + i18n/UI Framework

## Five-Agent Workflow (Iteration 3 — complete)

| Agent | Role | Deliverables |
|-------|------|--------------|
| **1 Planner** | Scope Iteration 3 vs `PHASE3` backlog | Task breakdown, risk list, this doc |
| **2 i18n** | New UI strings + TS contexts | `ResultRow`, tray index messages (zh/en) |
| **3 Implementer** | Icon cache, pin toggle, index toast | `IconCache`, `AppScanner`, overlay UX |
| **4 Reviewer** | Spec + standards check | No scope creep, ADR compliance |
| **5 Tester** | Automated + manual matrix | `AppScannerTest`, `IconCacheTest`, pin test |

### Anti-lazy rules (all agents)

- **禁止**只做占位 UI、不接线 i18n
- **禁止**硬编码颜色/字符串，必须用 Theme + `tr()`/`qsTr()`
- **禁止**跳过分语言切换验证（中/英各测一遍）
- **禁止**把设置页所有控件堆在一个 Tab（保持 General / Hotkeys / Clipboard）
- **禁止**未更新文档就宣称 Phase 完成
- **禁止** QML 翻译 context 写错（必须用 QML 文件名）
- **禁止**图标缓存写了但不显示在 ResultRow
- **禁止** Pin 功能没有键盘快捷键且无视觉反馈
- **禁止**索引刷新成功/失败不通知用户

---

## Delivered in Iteration 3

- [x] **Icon cache pipeline** — `IconCache` writes PNG to cache dir on index refresh; `ResultRow` shows cached icon
- [x] **Pin toggle** — `Ctrl+Shift+P` in Search Mode; ★ badge on pinned rows
- [x] **Index refresh toast** — tray notification on user-triggered refresh (success if count > 0; failure localized)
- [x] **App scanner extraction** — `scan_lnk_roots()` testable independently of full Windows scan
- [x] **Tests** — `AppScannerTest`, `IconCacheTest`, `toggle_pin` in `LauncherControllerTest`

## Remaining (Phase 3 polish / V1.1)

- [ ] Pin toggle for clipboard entries
- [ ] Index refresh when catalog empty (show "0 indexed" toast option)
- [ ] Bundle `qt_zh_CN.qm` fallback for QMessageBox buttons
- [ ] User-facing error codes for all platform `Result` strings

---

## Manual Verification (user — Qt Creator)

1. Run CMake → Rebuild
2. Settings → **刷新应用索引** → tray shows **已索引 N 个应用**
3. `Alt+Space` → app rows show icons (after index refresh)
4. Select app → `Ctrl+Shift+P` → ★ appears; list re-sorts with pinned first
5. Language 简体中文 → overlay **已置顶** tooltip, tray messages in Chinese
6. Run tests: 9/9 should pass (2 new + existing)

## Test Matrix

| Test | Covers |
|------|--------|
| AppScannerTest | fixture directory scan, ignores non-.lnk |
| IconCacheTest | notepad.exe icon cache (Windows) |
| LauncherControllerTest | pin toggle |
| LocaleServiceTest | language normalize/display |
| SearchRankingTest | search scoring |
| (manual) | icons, pin, index toast, i18n |
