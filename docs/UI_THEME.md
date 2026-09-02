# QuickDeck UI Theme — Mint Sky (Light Glass)

## Design Intent

Fresh, light, airy launcher overlay with frosted-glass panels, soft shadows, and rounded geometry (no sharp corners).

## Tokens (`QuickDeckTheme.qml`)

| Token | Value | Usage |
|-------|-------|-------|
| `primary` | `#38BDF8` | Accent, focus ring |
| `primarySoft` | `#E0F2FE` | Badges, avatars |
| `accent` | `#5EEAD4` | Secondary highlight |
| `glassFill` | `#D9FFFFFF` | Panel background (~85% white) |
| `glassBorder` | `#CCFFFFFF` | Panel edge |
| `textPrimary` | `#0F172A` | Titles |
| `textSecondary` | `#64748B` | Subtitles |
| `radiusWindow` | `24` | Outer shell |
| `radiusControl` | `14` | Inputs, rows |

## Components

| File | Role |
|------|------|
| `theme/QuickDeckTheme.qml` | Singleton color/spacing tokens |
| `components/GlassPanel.qml` | Frosted card + shadow |
| `components/SearchField.qml` | Rounded search input |
| `components/ResultRow.qml` | List delegate with avatar pill |

## Widgets (Settings / Wizard)

Qt Widgets use a light QSS palette aligned with the same tokens (see `SettingsWindow::apply_style()`).

## Extending

- New QML screens: import `QuickDeckLauncher`, use `QuickDeckTheme.*` — do not hardcode hex values
- Dark mode: future `general.theme` setting can swap token values or load `QuickDeckThemeDark.qml`
