# Phase 4 — Clipboard UX, Quick Paste, Retention Feedback

## Five-Agent Workflow (complete)

| Agent | Role | Deliverables |
|-------|------|--------------|
| **1 Planner** | Scope vs V1 | Task breakdown, anti-lazy rules, this doc |
| **2 Implementer** | Quick paste + simulate paste | Hotkey, settings, `LauncherController`, platform hook |
| **3 Data/UX** | Retention feedback + core cleanup | `enforce_retention` count, tray toast, `search_ranking` → `core/` |
| **4 Reviewer** | Spec + standards | No scope creep, i18n wired |
| **5 Tester** | Automated coverage | `ClipboardRetentionTest`, quick paste + `UserMessages` paste codes |

### Anti-lazy rules (all agents)

- **禁止**只做 Settings 占位控件，不实现 `load_values` / `save_values` / 热键注册
- **禁止**`quick_paste` 失败无用户可见反馈（必须 tray + `UserMessages`）
- **禁止**保留 `enforce_retention` 静默删条目（必须 `entries_trimmed` → tray）
- **禁止**把 `search_ranking` 只改 include 不更新 CMake / 测试
- **禁止**硬编码快捷键说明字符串，必须 `tr()` + TS 更新
- **禁止**未更新 `PROJECT_STATUS.md` 就宣称 Phase 4 完成

---

## Delivered

- [x] **Quick paste hotkey** — default `Ctrl+Alt+V`, setting `clipboard.quick_paste_hotkey`, id `quick_paste`
- [x] **Simulate paste on overlay activate** — setting `clipboard.simulate_paste_on_activate`
- [x] **Retention tray toast** — `ClipboardMonitor::entries_trimmed` → `TrayManager::show_entries_trimmed`
- [x] **`enforce_retention` returns removed count** — `Result<int>` for trim + age deletes
- [x] **Error codes** — `paste.simulate_failed`, `paste.no_entries` in `UserMessages`
- [x] **`search_ranking` moved to `core/`** — resolves ARCHITECTURE debt item
- [x] **Tests** — 11/11 (`ClipboardRetentionTest`, quick paste cases, paste error translation)
- [x] **Version** — `0.5.0`

## Phase 4 Status

**Complete** (2026-09-02). See [PROJECT_STATUS.md](PROJECT_STATUS.md).

---

## Manual Verification (user — Qt Creator)

1. Rebuild → run tests (11/11 expected)
2. Settings → Hotkeys → **Quick paste hotkey** (default `Ctrl+Alt+V`)
3. Copy text → `Ctrl+Alt+V` in another app → latest history pasted (Win simulate paste)
4. Empty history → quick paste → tray「无剪贴板历史可粘贴」
5. Settings → Clipboard → enable **Simulate paste when re-copying from overlay** → pick clip in overlay
6. Fill history past max entries → tray shows removed count toast
7. Switch to 简体中文 — new settings + tray strings Chinese

## Test Matrix

| Test | Covers |
|------|--------|
| ClipboardRetentionTest | Oldest unpinned entries trimmed, count returned |
| LauncherControllerTest | quick paste empty / with entry |
| UserMessagesTest | paste error code translation |
| SearchRankingTest | includes from `core/search_ranking.h` |
