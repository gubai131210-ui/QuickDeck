#include "data/repositories/sqlite_clipboard_repository.h"

#include <QDateTime>
#include <QSqlError>
#include <QSqlQuery>

namespace quickdeck {

SqliteClipboardRepository::SqliteClipboardRepository(QSqlDatabase db)
    : db_(std::move(db))
{
}

QString SqliteClipboardRepository::make_preview(const QString &content) const
{
    QString preview = content;
    preview.replace('\n', ' ');
    if (preview.length() > 120) {
        preview = preview.left(117) + QStringLiteral("...");
    }
    return preview;
}

ClipboardEntry SqliteClipboardRepository::row_to_entry(const QSqlQuery &query) const
{
    ClipboardEntry entry;
    entry.id = query.value(QStringLiteral("id")).toLongLong();
    entry.content_type = query.value(QStringLiteral("content_type")).toString();
    entry.content = query.value(QStringLiteral("content")).toString();
    entry.preview = query.value(QStringLiteral("preview")).toString();
    entry.source_app = query.value(QStringLiteral("source_app")).toString();
    entry.is_pinned = query.value(QStringLiteral("is_pinned")).toInt() != 0;
    entry.created_at = query.value(QStringLiteral("created_at")).toLongLong();
    entry.updated_at = query.value(QStringLiteral("updated_at")).toLongLong();
    return entry;
}

Result<qint64> SqliteClipboardRepository::insert(const ClipboardEntry &entry)
{
    const qint64 now = QDateTime::currentSecsSinceEpoch();
    {
        QSqlQuery query(db_);
        query.prepare(QStringLiteral(
            "INSERT INTO clipboard_entries "
            "(content_type, content, preview, source_app, is_pinned, created_at, updated_at) "
            "VALUES (?, ?, ?, ?, ?, ?, ?)"));
        query.addBindValue(entry.content_type);
        query.addBindValue(entry.content);
        query.addBindValue(entry.preview.isEmpty() ? make_preview(entry.content) : entry.preview);
        query.addBindValue(entry.source_app);
        query.addBindValue(entry.is_pinned ? 1 : 0);
        query.addBindValue(now);
        query.addBindValue(now);
        if (!query.exec()) {
            return Result<qint64>::fail(query.lastError().text());
        }
    }

    QSqlQuery id_query(db_);
    if (!id_query.exec(QStringLiteral("SELECT last_insert_rowid()")) || !id_query.next()) {
        return Result<qint64>::fail(id_query.lastError().text());
    }
    return Result<qint64>::ok(id_query.value(0).toLongLong());
}

Result<QList<ClipboardEntry>> SqliteClipboardRepository::list_recent(int limit, int offset)
{
    QList<ClipboardEntry> pinned;
    const Result<QList<ClipboardEntry>> pinned_result = list_pinned();
    if (pinned_result.is_ok()) {
        pinned = pinned_result.value();
    }

    QSqlQuery query(db_);
    query.prepare(QStringLiteral(
        "SELECT * FROM clipboard_entries WHERE is_pinned = 0 "
        "ORDER BY created_at DESC LIMIT ? OFFSET ?"));
    query.addBindValue(limit);
    query.addBindValue(offset);
    if (!query.exec()) {
        return Result<QList<ClipboardEntry>>::fail(query.lastError().text());
    }

    QList<ClipboardEntry> entries = pinned;
    while (query.next()) {
        entries.append(row_to_entry(query));
    }
    return Result<QList<ClipboardEntry>>::ok(entries);
}

Result<QList<ClipboardEntry>> SqliteClipboardRepository::search(const QString &query_text,
                                                              const ClipboardSearchOptions &options)
{
    QList<ClipboardEntry> pinned;
    const Result<QList<ClipboardEntry>> pinned_result = list_pinned();
    if (pinned_result.is_ok()) {
        pinned = pinned_result.value();
    }
    if (query_text.trimmed().isEmpty()) {
        return list_recent(options.limit, options.offset);
    }

    QSqlQuery query(db_);
    query.prepare(QStringLiteral(
        "SELECT c.* FROM clipboard_entries c "
        "INNER JOIN clipboard_fts f ON f.rowid = c.id "
        "WHERE f MATCH ? AND c.is_pinned = 0 "
        "ORDER BY c.created_at DESC LIMIT ? OFFSET ?"));
    query.addBindValue(query_text + QStringLiteral("*"));
    query.addBindValue(options.limit);
    query.addBindValue(options.offset);
    if (!query.exec()) {
        return Result<QList<ClipboardEntry>>::fail(query.lastError().text());
    }

    QList<ClipboardEntry> entries = pinned;
    while (query.next()) {
        entries.append(row_to_entry(query));
    }
    return Result<QList<ClipboardEntry>>::ok(entries);
}

Result<QList<ClipboardEntry>> SqliteClipboardRepository::list_pinned()
{
    QSqlQuery query(db_);
    if (!query.exec(QStringLiteral(
            "SELECT * FROM clipboard_entries WHERE is_pinned = 1 ORDER BY created_at DESC"))) {
        return Result<QList<ClipboardEntry>>::fail(query.lastError().text());
    }

    QList<ClipboardEntry> entries;
    while (query.next()) {
        entries.append(row_to_entry(query));
    }
    return Result<QList<ClipboardEntry>>::ok(entries);
}

Result<void> SqliteClipboardRepository::set_pinned(qint64 id, bool pinned)
{
    QSqlQuery query(db_);
    query.prepare(QStringLiteral("UPDATE clipboard_entries SET is_pinned = ?, updated_at = ? WHERE id = ?"));
    query.addBindValue(pinned ? 1 : 0);
    query.addBindValue(QDateTime::currentSecsSinceEpoch());
    query.addBindValue(id);
    if (!query.exec()) {
        return Result<void>::fail(query.lastError().text());
    }
    return Result<void>::ok();
}

Result<void> SqliteClipboardRepository::remove(qint64 id)
{
    QSqlQuery query(db_);
    query.prepare(QStringLiteral("DELETE FROM clipboard_entries WHERE id = ?"));
    query.addBindValue(id);
    if (!query.exec()) {
        return Result<void>::fail(query.lastError().text());
    }
    return Result<void>::ok();
}

Result<void> SqliteClipboardRepository::clear_unpinned()
{
    QSqlQuery query(db_);
    if (!query.exec(QStringLiteral("DELETE FROM clipboard_entries WHERE is_pinned = 0"))) {
        return Result<void>::fail(query.lastError().text());
    }
    return Result<void>::ok();
}

Result<int> SqliteClipboardRepository::enforce_retention(int max_entries, int max_age_days)
{
    int removed = 0;

    if (max_age_days > 0) {
        const qint64 cutoff = QDateTime::currentSecsSinceEpoch() - max_age_days * 86400LL;
        QSqlQuery age_query(db_);
        age_query.prepare(QStringLiteral(
            "DELETE FROM clipboard_entries WHERE is_pinned = 0 AND created_at < ?"));
        age_query.addBindValue(cutoff);
        if (!age_query.exec()) {
            return Result<int>::fail(age_query.lastError().text());
        }
        removed += age_query.numRowsAffected();
    }

    if (max_entries <= 0) {
        return Result<int>::ok(removed);
    }

    QSqlQuery count_query(db_);
    if (!count_query.exec(QStringLiteral(
            "SELECT COUNT(*) FROM clipboard_entries WHERE is_pinned = 0")) ||
        !count_query.next()) {
        return Result<int>::fail(count_query.lastError().text());
    }

    const int unpinned_count = count_query.value(0).toInt();
    if (unpinned_count <= max_entries) {
        return Result<int>::ok(removed);
    }

    const int delete_count = unpinned_count - max_entries;
    QSqlQuery trim_query(db_);
    trim_query.prepare(QStringLiteral(
        "DELETE FROM clipboard_entries WHERE id IN ("
        "  SELECT id FROM clipboard_entries WHERE is_pinned = 0 "
        "  ORDER BY created_at ASC LIMIT ?"
        ")"));
    trim_query.addBindValue(delete_count);
    if (!trim_query.exec()) {
        return Result<int>::fail(trim_query.lastError().text());
    }
    removed += trim_query.numRowsAffected();
    return Result<int>::ok(removed);
}

Result<bool> SqliteClipboardRepository::is_duplicate(const QString &content)
{
    QSqlQuery query(db_);
    query.prepare(QStringLiteral(
        "SELECT 1 FROM clipboard_entries WHERE content = ? ORDER BY created_at DESC LIMIT 1"));
    query.addBindValue(content);
    if (!query.exec()) {
        return Result<bool>::fail(query.lastError().text());
    }
    return Result<bool>::ok(query.next());
}

Result<int> SqliteClipboardRepository::count()
{
    QSqlQuery query(db_);
    if (!query.exec(QStringLiteral("SELECT COUNT(*) FROM clipboard_entries")) || !query.next()) {
        return Result<int>::fail(query.lastError().text());
    }
    return Result<int>::ok(query.value(0).toInt());
}

} // namespace quickdeck
