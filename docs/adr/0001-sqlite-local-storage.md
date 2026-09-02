# ADR 0001: SQLite for Local Persistence

## Status

Accepted

## Context

QuickDeck stores app catalog, clipboard history, usage statistics, and settings locally with no network upload. Data must survive restarts, support full-text search on clipboard content, and allow schema evolution across versions.

## Decision

Use SQLite via `QSqlDatabase` with numbered SQL migration files and a `schema_version` table.

## Consequences

- **Positive**: Single file database, FTS5 for clipboard search, Qt built-in support, easy backup.
- **Positive**: Migrations enable additive schema changes without wiping user data.
- **Negative**: SQLite write concurrency is limited; acceptable for a single-user desktop app.
- **Negative**: FTS5 triggers must stay in sync with `clipboard_entries` table changes.

## Alternatives Considered

- **JSON files**: Poor query performance at scale; rejected.
- **ORM libraries (sqlite_orm)**: Extra dependency; rejected for V1 simplicity.
