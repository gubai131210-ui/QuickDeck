#pragma once

#include <QString>

namespace quickdeck {

enum class EntityType {
    App,
    Clipboard,
    Command, // reserved for V1.1
    File     // reserved for V1.1
};

[[nodiscard]] inline QString entity_type_to_string(EntityType type)
{
    switch (type) {
    case EntityType::App:
        return QStringLiteral("app");
    case EntityType::Clipboard:
        return QStringLiteral("clipboard");
    case EntityType::Command:
        return QStringLiteral("command");
    case EntityType::File:
        return QStringLiteral("file");
    }
    return QStringLiteral("unknown");
}

[[nodiscard]] inline EntityType entity_type_from_string(const QString &value)
{
    if (value == QStringLiteral("app"))
        return EntityType::App;
    if (value == QStringLiteral("clipboard"))
        return EntityType::Clipboard;
    if (value == QStringLiteral("command"))
        return EntityType::Command;
    if (value == QStringLiteral("file"))
        return EntityType::File;
    return EntityType::App;
}

struct AppEntry {
    qint64 id = 0;
    QString name;
    QString executable_path;
    QString icon_cache_path;
    QString launch_args;
    QString working_dir;
    QString platform_id = QStringLiteral("win");
    bool is_pinned = false;
    qint64 source_mtime = 0;
    qint64 indexed_at = 0;
    qint64 created_at = 0;
    qint64 updated_at = 0;
    int launch_count = 0;
    qint64 last_used_at = 0;
};

struct ClipboardEntry {
    qint64 id = 0;
    QString content_type = QStringLiteral("text/plain");
    QString content;
    QString preview;
    QString source_app;
    bool is_pinned = false;
    qint64 created_at = 0;
    qint64 updated_at = 0;
};

struct UsageRecord {
    qint64 id = 0;
    EntityType entity_type = EntityType::App;
    qint64 entity_id = 0;
    int launch_count = 0;
    qint64 last_used_at = 0;
};

enum class LauncherMode {
    Search,
    Clipboard
};

} // namespace quickdeck
