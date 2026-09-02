#include "data/database_manager.h"

#include "data/migration_runner.h"
#include "data/repositories/sqlite_app_repository.h"
#include "data/repositories/sqlite_clipboard_repository.h"
#include "data/repositories/sqlite_settings_store.h"

#include <QDir>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>

namespace quickdeck {

DatabaseManager::DatabaseManager(QObject *parent)
    : QObject(parent)
    , connection_name_(QStringLiteral("quickdeck_main_%1").arg(reinterpret_cast<quintptr>(this)))
{
}

DatabaseManager::~DatabaseManager()
{
    if (QSqlDatabase::contains(connection_name_)) {
        db_.close();
        QSqlDatabase::removeDatabase(connection_name_);
    }
}

QString DatabaseManager::default_database_path()
{
    const QString data_dir =
        QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(data_dir);
    return data_dir + QStringLiteral("/data.db");
}

Result<void> DatabaseManager::open(const QString &database_path,
                                   const QString &migrations_dir)
{
    if (QSqlDatabase::contains(connection_name_)) {
        db_.close();
        QSqlDatabase::removeDatabase(connection_name_);
    }

    db_ = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), connection_name_);
    db_.setDatabaseName(database_path);

    if (!db_.open()) {
        return Result<void>::fail(db_.lastError().text());
    }

    QSqlQuery pragma(db_);
    pragma.exec(QStringLiteral("PRAGMA foreign_keys = ON"));
    pragma.exec(QStringLiteral("PRAGMA journal_mode = WAL"));

    const Result<void> migration_result = MigrationRunner::apply_all(db_, migrations_dir);
    if (migration_result.is_err()) {
        return migration_result;
    }

    app_repo_ = std::make_unique<SqliteAppRepository>(db_);
    clipboard_repo_ = std::make_unique<SqliteClipboardRepository>(db_);
    settings_repo_ = std::make_unique<SqliteSettingsStore>(db_);

    return Result<void>::ok();
}

QSqlDatabase DatabaseManager::database() const
{
    return db_;
}

IAppRepository &DatabaseManager::apps()
{
    return *app_repo_;
}

IClipboardRepository &DatabaseManager::clipboard()
{
    return *clipboard_repo_;
}

ISettingsStore &DatabaseManager::settings()
{
    return *settings_repo_;
}

} // namespace quickdeck
