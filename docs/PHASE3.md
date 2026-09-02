# Phase 3 — App Indexer, Search, Launch + i18n/UI Framework

## Five-Agent Workflow (Iteration 4 — complete)

| Agent | Role | Deliverables |
|-------|------|--------------|
| **1 Planner** | Scope polish vs V1 | Task breakdown, risk list, this doc |
| **2 i18n** | Error codes + Qt fallback + new strings | `UserMessages`, `QMessageBox`, tray copy |
| **3 Implementer** | Clipboard pin, index 0 toast, user refresh | `refresh_catalog(true)`, overlay shortcut |
| **4 Reviewer** | Spec + standards check | No scope creep |
| **5 Tester** | Automated + manual matrix | `UserMessagesTest`, clipboard pin test |

### Anti-lazy rules (all agents)

- **禁止**只做占位 UI、不接线 i18n
- **禁止**硬编码颜色/字符串，必须用 Theme + `tr()`/`qsTr()`
- **禁止**跳过分语言切换验证（中/英各测一遍）
- **禁止**错误信息直接暴露 SQLite/Win32 原文给用户
- **禁止**启动时弹索引 toast（仅用户主动刷新时通知）
- **禁止**剪贴板 Pin 没有快捷键和 ★ 视觉反馈
- **禁止**未更新文档就宣称 Iteration 完成

---

## Delivered in Iteration 4

- [x] **Clipboard pin** — `Ctrl+Shift+P` in Clipboard Mode; ★ badge; selection restored by id
- [x] **Index 0 toast** — user refresh shows「未索引到任何应用」when catalog empty
- [x] **User-initiated refresh only** — `refresh_catalog(true)` from Settings; startup silent
- [x] **User-facing error codes** — `UserMessages` maps `launch.*` / `index.*` / `path.*`
- [x] **Qt zh_CN fallback** — embed `qt_zh_CN.qm` from Qt kit when present (`:/qt/i18n/`)
- [x] **Tests** — `UserMessagesTest`, clipboard pin in `LauncherControllerTest` (10/10)

## Phase 3 Status

**Complete** (Iterations 1–4, 2026-09-02). See [PROJECT_STATUS.md](PROJECT_STATUS.md) for V1 checklist and next work.

---

## Manual Verification (user — Qt Creator)

1. Run CMake → Rebuild (qt_zh_CN.qm may embed if Qt translations exist)
2. Settings → 刷新应用索引 → tray shows count or「未索引到任何应用」
3. `Ctrl+Shift+V` → select clip → `Ctrl+Shift+P` → ★ appears
4. Launch missing exe → tray shows localized error (not raw English path)
5. Settings hotkey conflict dialog → Yes/No in Chinese when locale is zh_CN
6. Tests: 10/10 pass

## Test Matrix

| Test | Covers |
|------|--------|
| UserMessagesTest | error code detection + translation |
| LauncherControllerTest | app pin + clipboard pin |
| AppScannerTest | lnk scan fixtures |
| IconCacheTest | icon PNG cache |
| LocaleServiceTest | language switching |
| (manual) | index 0 toast, clipboard pin, QMessageBox zh |
