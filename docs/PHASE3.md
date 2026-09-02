# Phase 3 — App Indexer, Search, Launch + i18n/UI Framework

## Five-Agent Workflow

| Agent | Role | Deliverables |
|-------|------|--------------|
| **1 Planner** | Research + scope vs `V1_SCOPE.md` | This doc, task breakdown, risk list |
| **2 i18n/Theme** | Locale framework + visual system | `LocaleService`, TS files, `QuickDeckTheme`, QML components |
| **3 Implementer** | Core Phase 3 features | Indexer polish, launch path, overlay UX |
| **4 Reviewer** | Spec + standards check | ADR compliance, no scope creep |
| **5 Tester** | CRAP + reverse tests | `LocaleServiceTest`, indexer tests, manual matrix |

### Anti-lazy rules (all agents)

- **禁止**只做占位 UI、不接线 i18n
- **禁止**硬编码颜色/字符串，必须用 Theme + `tr()`/`qsTr()`
- **禁止**跳过分语言切换验证（中/英各测一遍）
- **禁止**把设置页所有控件堆在一个 Tab（保持 General / Hotkeys / Clipboard）
- **禁止**未更新文档就宣称 Phase 完成

## Delivered in This Phase (framework)

- [x] `LocaleService` + en/zh_CN translations embedded in exe
- [x] Settings language switch with live QML retranslate
- [x] Light glass overlay (`GlassPanel`, `SearchField`, `ResultRow`)
- [x] Empty-state messages + item count badge
- [x] `win_lnk_resolver` (Windows COM, ready for launch/display polish)
- [x] `LocaleServiceTest`

## Remaining Phase 3 (next iterations)

- [ ] App icon cache pipeline
- [ ] Pin / usage-weighted sort in overlay
- [ ] Launch failure toast in tray
- [ ] `.lnk` target shown in subtitle without breaking UNIQUE key
- [ ] Indexer integration tests with fixture shortcuts

## Manual Verification (user — Qt Creator)

1. Rebuild (CMake changed — Run CMake first)
2. Settings → Language → 简体中文 → Save → overlay strings中文
3. Switch back to English → overlay English
4. `Alt+Space` — light glass panel, rounded corners, soft shadow
5. Empty catalog shows “No applications found”
6. Refresh index in Settings — apps appear after scan

## Test Matrix

| Test | Covers |
|------|--------|
| LocaleServiceTest | language normalize/display |
| SearchRankingTest | search scoring |
| LauncherControllerTest | overlay controller |
| (manual) | i18n switch, glass UI, app launch |
