# Phase 5 — First-Run Polish, Startup Performance, Release Packaging

## Five-Agent Workflow (complete)

| Agent | Role | Deliverables |
|-------|------|--------------|
| **1 Planner** | V1 release scope | Task breakdown, anti-lazy rules, this doc |
| **2 UX** | First-run wizard polish | Language pick, quick paste hotkey, duplicate check, summary |
| **3 Performance** | Startup responsiveness | Defer background app index to next event loop tick |
| **4 Release** | Windows packaging | `scripts/package_windows.ps1`, `docs/RELEASE.md`, CMake target |
| **5 Tester/Docs** | Status + manual matrix | README/AGENTS/V1_SCOPE sync, version `0.6.0` |

### Anti-lazy rules (all agents)

- **禁止**向导只加 UI 不持久化语言 / 快速粘贴快捷键
- **禁止**只做文档不写可执行的 `package_windows.ps1`
- **禁止**启动索引仍阻塞主线程（必须 defer 到事件循环）
- **禁止**向导与 Settings 使用不同的 settings key（语言必须是 `general.language`）
- **禁止**重复快捷键无校验
- **禁止**未更新 `PROJECT_STATUS.md` / `README.md` 就宣称 Phase 5 完成

---

## Delivered

- [x] **First-run wizard** — language combo, launcher + clipboard + quick paste hotkeys, duplicate/conflict checks, ready summary
- [x] **Deferred startup indexing** — `QTimer::singleShot(0)` so tray/hotkeys register before scan
- [x] **Windows packaging** — PowerShell script + `QuickDeckPackage` CMake target + RELEASE guide
- [x] **Docs sync** — README, AGENTS, V1_SCOPE (quick paste in scope), PROJECT_STATUS
- [x] **Version** — `0.6.0`

## Phase 5 Status

**Complete** (2026-09-02). V1 feature-complete; release packaging documented for user.

---

## Manual Verification (user — Qt Creator)

1. Delete `%APPDATA%/QuickDeck/data.db` or reset `setup.completed` → first-run wizard shows language + 3 hotkeys
2. Pick 简体中文 in wizard → UI switches before finish
3. Set duplicate hotkeys → warning on finish
4. App starts → tray appears immediately; index runs in background (log: "Indexed N applications")
5. Package (from build dir, adjust paths):

```powershell
cmake --build . --target QuickDeckPackage
# or:
.\scripts\package_windows.ps1 -BuildDir "D:\path\to\build\QuickDeck" -QtBinDir "D:\QT\6.11.1\mingw_64\bin"
```

6. Run `QuickDeck-dist\QuickDeck.exe` on a clean folder — overlay + hotkeys work

## Test Matrix

| Area | Covers |
|------|--------|
| Existing 11 tests | No regressions |
| First-run (manual) | Language, 3 hotkeys, duplicate/conflict |
| Packaging (manual) | windeployqt bundle runs offline |
