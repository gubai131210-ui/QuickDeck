#include "data/migration_runner.h"

#include <QDir>
#include <QFile>
#include <QSqlError>
#include <QSqlQuery>

namespace quickdeck {

Result<int> MigrationRunner::current_version(QSqlDatabase &db)
{
    QSqlQuery probe(db);
    if (!probe.exec(QStringLiteral("SELECT name FROM sqlite_master WHERE type='table' AND name='schema_version'"))) {
        return Result<int>::fail(probe.lastError().text());
    }
    if (!probe.next()) {
        return Result<int>::ok(0);
    }

    QSqlQuery query(db);
    if (!query.exec(QStringLiteral("SELECT MAX(version) FROM schema_version"))) {
        return Result<int>::fail(query.lastError().text());
    }
    if (query.next() && !query.value(0).isNull()) {
        return Result<int>::ok(query.value(0).toInt());
    }
    return Result<int>::ok(0);
}

Result<void> MigrationRunner::apply_file(QSqlDatabase &db, const QString &file_path)
{
    QFile file(file_path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return Result<void>::fail(
            QStringLiteral("Cannot open migration: %1").arg(file_path));
    }

    const QString sql = QString::fromUtf8(file.readAll());
    QSqlQuery query(db);
    if (!query.exec(sql)) {
        return Result<void>::fail(
            QStringLiteral("Migration failed in %1: %2")
                .arg(file_path, query.lastError().text()));
    }
    return Result<void>::ok();
}

Result<void> MigrationRunner::apply_all(QSqlDatabase &db, const QString &migrations_dir)
{
    const Result<int> version_result = current_version(db);
    if (version_result.is_err()) {
        return Result<void>::fail(version_result.error());
    }
    int current = version_result.value();

    QDir dir(migrations_dir);
    if (!dir.exists()) {
        return Result<void>::fail(
            QStringLiteral("Migrations directory not found: %1").arg(migrations_dir));
    }

    const QStringList files =
        dir.entryList({QStringLiteral("*.sql")}, QDir::Files, QDir::Name);
    for (const QString &file_name : files) {
        const int migration_number = file_name.left(3).toInt();
        if (migration_number <= current) {
            continue;
        }

        const QString file_path = dir.absoluteFilePath(file_name);
        const Result<void> apply_result = apply_file(db, file_path);
        if (apply_result.is_err()) {
            return apply_result;
        }

        if (!db.transaction()) {
            return Result<void>::fail(db.lastError().text());
        }
        QSqlQuery query(db);
        query.prepare(QStringLiteral("INSERT INTO schema_version (version) VALUES (?)"));
        query.addBindValue(migration_number);
        if (!query.exec()) {
            db.rollback();
            return Result<void>::fail(query.lastError().text());
        }
        if (!db.commit()) {
            db.rollback();
            return Result<void>::fail(db.lastError().text());
        }
        current = migration_number;
    }

    return Result<void>::ok();
}

} // namespace quickdeck
