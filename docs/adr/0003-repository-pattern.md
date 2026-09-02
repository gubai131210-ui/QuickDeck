# ADR 0003: Repository Pattern at Data Seam

## Status

Accepted

## Context

Services (indexer, clipboard monitor, search) must not depend on SQL details. Tests need in-memory or fake stores. Future schema changes should not ripple into business logic.

## Decision

Define repository **interfaces** in `src/core/interfaces/`. Implement SQLite adapters in `src/data/repositories/`. Inject repositories through `ApplicationContext`.

## Consequences

- **Positive**: Deep module — small interface, complex SQL hidden.
- **Positive**: Unit tests can use fakes without a database file.
- **Negative**: More boilerplate than direct SQL in services; justified by extensibility.

## Interface Surface

- `IAppRepository` — catalog CRUD, search, pin, usage
- `IClipboardRepository` — history CRUD, FTS search, pin, cleanup
- `ISettingsStore` — typed key-value access
