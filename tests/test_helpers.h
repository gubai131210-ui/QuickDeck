#pragma once

#include "app/application_context.h"

#ifndef QUICKDECK_SOURCE_DIR
#define QUICKDECK_SOURCE_DIR "."
#endif

#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QString>

namespace quickdeck::test {

inline QString migrations_directory()
{
    const QStringList candidates = {
        QDir(QCoreApplication::applicationDirPath()).filePath(QStringLiteral("migrations")),
        QStringLiteral(QUICKDECK_SOURCE_DIR "/migrations"),
    };
    for (const QString &candidate : candidates) {
        if (QDir(candidate).exists()) {
            return candidate;
        }
    }
    return candidates.last();
}

inline Result<void> initialize_isolated_context(ApplicationContext &context,
                                                const QString &database_path)
{
    return context.initialize(database_path);
}

inline Result<void> seed_clipboard_text(QSqlDatabase db, const QString &content)
{
    const qint64 now = QDateTime::currentSecsSinceEpoch();
    QSqlQuery query(db);
    query.prepare(QStringLiteral(
        "INSERT INTO clipboard_entries "
        "(content_type, content, preview, source_app, is_pinned, created_at, updated_at) "
        "VALUES (?, ?, ?, ?, ?, ?, ?)"));
    query.addBindValue(QStringLiteral("text/plain"));
    query.addBindValue(content);
    query.addBindValue(content);
    query.addBindValue(QString());
    query.addBindValue(0);
    query.addBindValue(now);
    query.addBindValue(now);
    if (!query.exec()) {
        return Result<void>::fail(query.lastError().text());
    }
    return Result<void>::ok();
}

} // namespace quickdeck::test
