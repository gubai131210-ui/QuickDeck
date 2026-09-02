# Phase 6 — UX Overhaul (V1.1)

## Five-Agent Workflow

| Agent | Role | Deliverables |
|-------|------|--------------|
| **1 Planner** | Scope lock | This doc; no theme, no installer |
| **2 Panel UX** | Dual-page shell + motion | `SearchPage`, `ClipboardPage`, `ModeSwitcher`, stagger animations |
| **3 Command layer** | Input-as-action | `core/command_router`, `CommandSearchModel`, `>` builtins |
| **4 Settings QML** | Visual unity | `SettingsController`, `SettingsOverlay.qml` replaces Widget dialog |
| **5 Debt + tests** | V1 consensus gaps | Ctrl+Enter paste, Delete clip, debug log, log rotate, hotkey tray, `CommandRouterTest` |

### Anti-lazy rules

- **禁止**只改标题文字冒充双页 — 必须 `StackLayout` + 独立 page QML
- **禁止**命令面板只做 UI — `CommandRouter` + `activate_selected` 必须执行
- **禁止**Settings 继续用 Widget QDialog 作为主入口
- **禁止**动效仅 opacity — 模式切换 + 列表 stagger 必须可见
- **禁止**跳过 Ctrl+Enter / Delete / debug log / 热键失败 toast
- **禁止**本阶段做主题 dark/system（用户指定暂缓）

---

## Delivered

- [x] **Dual-page overlay** — Search / Clipboard pages, tab switcher, Tab key, slide fade
- [x] **List motion** — staggered row entrance, mode transition fade
- [x] **Command palette V1** — `>settings`, `>lang`, `>refresh`, `>search`, `>clipboard`, `>paste`
- [x] **Settings QML** — glass `SettingsOverlay`, `SettingsController` C++ backend
- [x] **Shortcut hints** — `ResultRow.actionHint`
- [x] **Ctrl+Enter** — simulate paste in clipboard mode
- [x] **Delete** — remove clipboard entry
- [x] **Debug log setting** + **5MB log rotation**
- [x] **Hotkey registration failure** — tray toast
- [x] **Version** — `0.7.0`
- [x] **Tests** — 12/12 (+ `CommandRouterTest`)

## Deferred (explicit)

- Theme dark / follow system
- Installer / packaging polish
- First-run wizard QML migration

---

## Manual Verification (user — Qt Creator)

1. Rebuild → `ctest` (12/12)
2. `Alt+Space` → tab **Search | Clipboard** switches with animation
3. Type `>` → command list; `>settings` opens QML settings
4. Clipboard mode: **Enter** copy, **Ctrl+Enter** paste, **Delete** removes entry
5. Tray → Settings → same glass QML panel
6. Settings → enable debug log → check `%APPDATA%/QuickDeck/logs/quickdeck.log`
