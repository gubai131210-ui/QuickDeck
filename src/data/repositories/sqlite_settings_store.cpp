#include "data/repositories/sqlite_settings_store.h"

#include <QSqlError>
#include <QSqlQuery>

namespace quickdeck {

SqliteSettingsStore::SqliteSettingsStore(QSqlDatabase db)
    : db_(std::move(db))
{
}

Result<QString> SqliteSettingsStore::get_string(const QString &key, const QString &default_value)
{
    QSqlQuery query(db_);
    query.prepare(QStringLiteral("SELECT value FROM settings WHERE key = ?"));
    query.addBindValue(key);
    if (!query.exec()) {
        return Result<QString>::fail(query.lastError().text());
    }
    if (query.next()) {
        return Result<QString>::ok(query.value(0).toString());
    }
    return Result<QString>::ok(default_value);
}

Result<int> SqliteSettingsStore::get_int(const QString &key, int default_value)
{
    const Result<QString> value = get_string(key, QString::number(default_value));
    if (value.is_err()) {
        return Result<int>::fail(value.error());
    }
    return Result<int>::ok(value.value().toInt());
}

Result<bool> SqliteSettingsStore::get_bool(const QString &key, bool default_value)
{
    const Result<QString> value =
        get_string(key, default_value ? QStringLiteral("true") : QStringLiteral("false"));
    if (value.is_err()) {
        return Result<bool>::fail(value.error());
    }
    const QString normalized = value.value().trimmed().toLower();
    return Result<bool>::ok(normalized == QStringLiteral("true") || normalized == QStringLiteral("1"));
}

Result<void> SqliteSettingsStore::upsert(const QString &key,
                                       const QString &value,
                                       const QString &value_type)
{
    QSqlQuery query(db_);
    query.prepare(QStringLiteral(
        "INSERT INTO settings (key, value, value_type) VALUES (?, ?, ?) "
        "ON CONFLICT(key) DO UPDATE SET value = excluded.value, value_type = excluded.value_type"));
    query.addBindValue(key);
    query.addBindValue(value);
    query.addBindValue(value_type);
    if (!query.exec()) {
        return Result<void>::fail(query.lastError().text());
    }
    return Result<void>::ok();
}

Result<void> SqliteSettingsStore::set_string(const QString &key, const QString &value)
{
    return upsert(key, value, QStringLiteral("string"));
}

Result<void> SqliteSettingsStore::set_int(const QString &key, int value)
{
    return upsert(key, QString::number(value), QStringLiteral("int"));
}

Result<void> SqliteSettingsStore::set_bool(const QString &key, bool value)
{
    return upsert(key, value ? QStringLiteral("true") : QStringLiteral("false"),
                  QStringLiteral("bool"));
}

Result<void> SqliteSettingsStore::reset_to_defaults()
{
    QSqlQuery query(db_);
    if (!query.exec(QStringLiteral("DELETE FROM settings"))) {
        return Result<void>::fail(query.lastError().text());
    }

    query.exec(QStringLiteral(
        "INSERT INTO settings (key, value, value_type) VALUES "
        "('launcher.hotkey', 'Alt+Space', 'string'), "
        "('clipboard.hotkey', 'Ctrl+Shift+V', 'string'), "
        "('clipboard.max_entries', '200', 'int'), "
        "('clipboard.max_char_length', '10000', 'int'), "
        "('clipboard.monitoring_enabled', 'true', 'bool'), "
        "('general.auto_start', 'false', 'bool'), "
        "('general.close_on_blur', 'true', 'bool'), "
        "('general.theme', 'system', 'string'), "
        "('indexer.last_refresh_at', '0', 'int'), "
        "('setup.completed', 'false', 'bool')"));
    return Result<void>::ok();
}

} // namespace quickdeck
