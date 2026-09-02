# QuickDeck Domain Glossary

Ubiquitous language for QuickDeck. Implementation details belong in code and ADRs, not here.

## Product

**QuickDeck** — A desktop efficiency tool combining an application launcher and clipboard history manager.

**Launcher** — The search-driven module for finding and opening applications, files by path, and (in later versions) custom commands.

**Clipboard History** — The module that records, searches, and reuses previously copied text content.

## Modes

**Search Mode** — Launcher overlay focused on application and path lookup. Invoked by the primary launcher hotkey.

**Clipboard Mode** — Launcher overlay focused on clipboard history. Invoked by the clipboard panel hotkey.

Both modes share one overlay window; mode is determined by which hotkey opened it.

## Entities

**App Entry** — A discoverable installed application with a display name, executable path, and optional icon.

**Clipboard Entry** — A recorded clip of copied content with preview text, timestamp, and optional source application.

**Usage Record** — Launch or reuse statistics tied to a typed entity (`app`, `clipboard`, etc.).

**Pinned** — A user mark keeping an app or clipboard entry at the top of its list and exempt from automatic cleanup.

## Actions

**Launch** — Start an application executable from an app entry.

**Path Open** — Open a file or folder when the user types an absolute or home-relative path in Search Mode.

**Re-copy** — Put a clipboard entry's content back onto the system clipboard.

**Simulated Paste** — Inject a paste keystroke into the active window after re-copying (Windows only in V1).

**Index Refresh** — Rescan installed applications and update the app catalog.

## Boundaries

**Catalog** — The set of indexed app entries available for search.

**History** — The ordered set of clipboard entries stored locally.

**Settings** — User preferences stored as key-value pairs; not part of catalog or history.

## Error Messages

**Error Code** — Stable machine identifier (e.g. `launch.file_missing`) returned in `Result` and translated for UI via `UserMessages`. Log lines may retain technical detail; tray/dialog text uses codes only.
