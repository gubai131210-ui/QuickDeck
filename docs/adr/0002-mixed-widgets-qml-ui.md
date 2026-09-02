# ADR 0002: Mixed Widgets and QML UI

## Status

Accepted

## Context

The launcher overlay needs frameless windows, transparency, animations, and dynamic height. Settings and tray fit traditional form UIs. The project started as Qt Widgets.

## Decision

Use **Qt Widgets** for system tray and settings dialogs. Use **Qt Quick (QML)** for the launcher overlay. Bridge via `QAbstractListModel` subclasses exposed to QML context properties.

## Consequences

- **Positive**: QML suits overlay UX; Widgets suit settings forms and tray menus.
- **Positive**: Core logic stays in `QuickDeckCore` static library, UI-agnostic.
- **Negative**: Two UI paradigms to maintain; mitigated by thin UI layer.

## Alternatives Considered

- **Widgets only**: Harder to achieve modern overlay aesthetics.
- **QML only**: Settings forms slower to build in V1.
