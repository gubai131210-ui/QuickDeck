-- QuickDeck schema v1
-- Applied by MigrationRunner as migration 001

PRAGMA foreign_keys = ON;

CREATE TABLE IF NOT EXISTS schema_version (
    version     INTEGER NOT NULL
);

CREATE TABLE IF NOT EXISTS apps (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    name            TEXT    NOT NULL,
    executable_path TEXT    NOT NULL UNIQUE,
    icon_cache_path TEXT,
    launch_args     TEXT,
    working_dir     TEXT,
    platform_id     TEXT    NOT NULL DEFAULT 'win',
    is_pinned       INTEGER NOT NULL DEFAULT 0 CHECK (is_pinned IN (0, 1)),
    source_mtime    INTEGER,
    indexed_at      INTEGER NOT NULL,
    created_at      INTEGER NOT NULL,
    updated_at      INTEGER NOT NULL
);

CREATE INDEX IF NOT EXISTS idx_apps_name ON apps(name);
CREATE INDEX IF NOT EXISTS idx_apps_pinned ON apps(is_pinned);

CREATE TABLE IF NOT EXISTS clipboard_entries (
    id           INTEGER PRIMARY KEY AUTOINCREMENT,
    content_type TEXT    NOT NULL DEFAULT 'text/plain',
    content      TEXT    NOT NULL,
    preview      TEXT    NOT NULL,
    source_app   TEXT,
    is_pinned    INTEGER NOT NULL DEFAULT 0 CHECK (is_pinned IN (0, 1)),
    created_at   INTEGER NOT NULL,
    updated_at   INTEGER NOT NULL
);

CREATE INDEX IF NOT EXISTS idx_clipboard_created ON clipboard_entries(created_at DESC);
CREATE INDEX IF NOT EXISTS idx_clipboard_pinned ON clipboard_entries(is_pinned);

CREATE VIRTUAL TABLE IF NOT EXISTS clipboard_fts USING fts5(
    content,
    preview,
    content='clipboard_entries',
    content_rowid='id',
    tokenize='unicode61 remove_diacritics 0'
);

CREATE TRIGGER IF NOT EXISTS clipboard_entries_ai AFTER INSERT ON clipboard_entries BEGIN
    INSERT INTO clipboard_fts(rowid, content, preview)
    VALUES (new.id, new.content, new.preview);
END;

CREATE TRIGGER IF NOT EXISTS clipboard_entries_ad AFTER DELETE ON clipboard_entries BEGIN
    INSERT INTO clipboard_fts(clipboard_fts, rowid, content, preview)
    VALUES ('delete', old.id, old.content, old.preview);
END;

CREATE TRIGGER IF NOT EXISTS clipboard_entries_au AFTER UPDATE ON clipboard_entries BEGIN
    INSERT INTO clipboard_fts(clipboard_fts, rowid, content, preview)
    VALUES ('delete', old.id, old.content, old.preview);
    INSERT INTO clipboard_fts(rowid, content, preview)
    VALUES (new.id, new.content, new.preview);
END;

CREATE TABLE IF NOT EXISTS usage_stats (
    id           INTEGER PRIMARY KEY AUTOINCREMENT,
    entity_type  TEXT    NOT NULL,
    entity_id    INTEGER NOT NULL,
    launch_count INTEGER NOT NULL DEFAULT 0,
    last_used_at INTEGER NOT NULL,
    UNIQUE (entity_type, entity_id)
);

CREATE INDEX IF NOT EXISTS idx_usage_entity ON usage_stats(entity_type, entity_id);

CREATE TABLE IF NOT EXISTS settings (
    key        TEXT PRIMARY KEY NOT NULL,
    value      TEXT NOT NULL,
    value_type TEXT NOT NULL DEFAULT 'string'
        CHECK (value_type IN ('string', 'int', 'bool', 'json'))
);

-- Default settings (extensible via new keys without schema change)
INSERT OR IGNORE INTO settings (key, value, value_type) VALUES
    ('launcher.hotkey',           'Alt+Space',       'string'),
    ('clipboard.hotkey',          'Ctrl+Shift+V',    'string'),
    ('clipboard.max_entries',     '200',             'int'),
    ('clipboard.max_char_length',   '10000',           'int'),
    ('clipboard.monitoring_enabled','true',           'bool'),
    ('general.auto_start',          'false',           'bool'),
    ('general.close_on_blur',       'true',            'bool'),
    ('general.theme',               'system',          'string'),
    ('indexer.last_refresh_at',     '0',               'int'),
    ('setup.completed',             'false',           'bool');
