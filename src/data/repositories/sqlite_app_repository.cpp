#include "data/repositories/sqlite_app_repository.h"

#include "services/search_ranking.h"

#include <QDateTime>
#include <QSet>
#include <QSqlError>
#include <QSqlQuery>

#include <algorithm>

namespace quickdeck {

SqliteAppRepository::SqliteAppRepository(QSqlDatabase db)
    : db_(std::move(db))
{
}

AppEntry SqliteAppRepository::row_to_entry(const QSqlQuery &query) const
{
    AppEntry entry;
    entry.id = query.value(QStringLiteral("id")).toLongLong();
    entry.name = query.value(QStringLiteral("name")).toString();
    entry.executable_path = query.value(QStringLiteral("executable_path")).toString();
    entry.icon_cache_path = query.value(QStringLiteral("icon_cache_path")).toString();
    entry.launch_args = query.value(QStringLiteral("launch_args")).toString();
    entry.working_dir = query.value(QStringLiteral("working_dir")).toString();
    entry.platform_id = query.value(QStringLiteral("platform_id")).toString();
    entry.is_pinned = query.value(QStringLiteral("is_pinned")).toInt() != 0;
    entry.source_mtime = query.value(QStringLiteral("source_mtime")).toLongLong();
    entry.indexed_at = query.value(QStringLiteral("indexed_at")).toLongLong();
    entry.created_at = query.value(QStringLiteral("created_at")).toLongLong();
    entry.updated_at = query.value(QStringLiteral("updated_at")).toLongLong();
    entry.launch_count = query.value(QStringLiteral("launch_count")).toInt();
    entry.last_used_at = query.value(QStringLiteral("last_used_at")).toLongLong();
    return entry;
}

Result<void> SqliteAppRepository::upsert(const AppEntry &entry)
{
    return upsert_batch({entry});
}

Result<void> SqliteAppRepository::upsert_batch(const QList<AppEntry> &entries)
{
    if (!db_.transaction()) {
        return Result<void>::fail(db_.lastError().text());
    }

    QSqlQuery query(db_);
    query.prepare(QStringLiteral(
        "INSERT INTO apps (name, executable_path, icon_cache_path, launch_args, working_dir, "
        "platform_id, is_pinned, source_mtime, indexed_at, created_at, updated_at) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?) "
        "ON CONFLICT(executable_path) DO UPDATE SET "
        "name=excluded.name, icon_cache_path=excluded.icon_cache_path, "
        "launch_args=excluded.launch_args, working_dir=excluded.working_dir, "
        "source_mtime=excluded.source_mtime, indexed_at=excluded.indexed_at, "
        "updated_at=excluded.updated_at"));

    const qint64 now = QDateTime::currentSecsSinceEpoch();
    for (const AppEntry &entry : entries) {
        query.addBindValue(entry.name);
        query.addBindValue(entry.executable_path);
        query.addBindValue(entry.icon_cache_path);
        query.addBindValue(entry.launch_args);
        query.addBindValue(entry.working_dir);
        query.addBindValue(entry.platform_id);
        query.addBindValue(entry.is_pinned ? 1 : 0);
        query.addBindValue(entry.source_mtime);
        query.addBindValue(entry.indexed_at > 0 ? entry.indexed_at : now);
        query.addBindValue(entry.created_at > 0 ? entry.created_at : now);
        query.addBindValue(now);
        if (!query.exec()) {
            db_.rollback();
            return Result<void>::fail(query.lastError().text());
        }
    }

    if (!db_.commit()) {
        db_.rollback();
        return Result<void>::fail(db_.lastError().text());
    }
    return Result<void>::ok();
}

Result<QList<AppEntry>> SqliteAppRepository::search(const QString &query_text,
                                                   const AppSearchOptions &options)
{
    QList<AppEntry> pinned;
    const Result<QList<AppEntry>> pinned_result = list_pinned();
    if (pinned_result.is_ok()) {
        pinned = pinned_result.value();
    }

    QSqlQuery query(db_);
    query.prepare(QStringLiteral(
        "SELECT a.*, COALESCE(u.launch_count, 0) AS launch_count, "
        "COALESCE(u.last_used_at, 0) AS last_used_at "
        "FROM apps a "
        "LEFT JOIN usage_stats u ON u.entity_type = 'app' AND u.entity_id = a.id "
        "ORDER BY a.name COLLATE NOCASE ASC"));
    if (!query.exec()) {
        return Result<QList<AppEntry>>::fail(query.lastError().text());
    }

    struct ScoredEntry {
        AppEntry entry;
        double score = 0.0;
    };

    QList<ScoredEntry> matches;
    const QString query_lower = query_text.trimmed().toLower();

    while (query.next()) {
        AppEntry entry = row_to_entry(query);
        if (query_lower.isEmpty()) {
            matches.append({entry, 0.0});
            continue;
        }
        const int match = match_score(entry.name, query_lower);
        if (match == 0) {
            continue;
        }
        matches.append({entry, final_search_score(match, entry.launch_count, entry.last_used_at)});
    }

    std::sort(matches.begin(), matches.end(), [](const ScoredEntry &a, const ScoredEntry &b) {
        return a.score > b.score;
    });

    QList<AppEntry> results = pinned;
    QSet<qint64> pinned_ids;
    for (const AppEntry &entry : pinned) {
        pinned_ids.insert(entry.id);
    }

    for (const ScoredEntry &scored : matches) {
        if (pinned_ids.contains(scored.entry.id)) {
            continue;
        }
        results.append(scored.entry);
        if (results.size() >= options.limit) {
            break;
        }
    }

    return Result<QList<AppEntry>>::ok(results);
}

Result<QList<AppEntry>> SqliteAppRepository::list_recent(int limit)
{
    QSqlQuery query(db_);
    query.prepare(QStringLiteral(
        "SELECT a.*, COALESCE(u.launch_count, 0) AS launch_count, "
        "COALESCE(u.last_used_at, 0) AS last_used_at "
        "FROM apps a "
        "INNER JOIN usage_stats u ON u.entity_type = 'app' AND u.entity_id = a.id "
        "ORDER BY u.last_used_at DESC LIMIT ?"));
    query.addBindValue(limit);
    if (!query.exec()) {
        return Result<QList<AppEntry>>::fail(query.lastError().text());
    }

    QList<AppEntry> entries;
    while (query.next()) {
        entries.append(row_to_entry(query));
    }
    return Result<QList<AppEntry>>::ok(entries);
}

Result<QList<AppEntry>> SqliteAppRepository::list_pinned()
{
    QSqlQuery query(db_);
    if (!query.exec(QStringLiteral(
            "SELECT a.*, COALESCE(u.launch_count, 0) AS launch_count, "
            "COALESCE(u.last_used_at, 0) AS last_used_at "
            "FROM apps a "
            "LEFT JOIN usage_stats u ON u.entity_type = 'app' AND u.entity_id = a.id "
            "WHERE a.is_pinned = 1 ORDER BY a.name COLLATE NOCASE ASC"))) {
        return Result<QList<AppEntry>>::fail(query.lastError().text());
    }

    QList<AppEntry> entries;
    while (query.next()) {
        entries.append(row_to_entry(query));
    }
    return Result<QList<AppEntry>>::ok(entries);
}

Result<void> SqliteAppRepository::set_pinned(qint64 id, bool pinned)
{
    QSqlQuery query(db_);
    query.prepare(QStringLiteral("UPDATE apps SET is_pinned = ?, updated_at = ? WHERE id = ?"));
    query.addBindValue(pinned ? 1 : 0);
    query.addBindValue(QDateTime::currentSecsSinceEpoch());
    query.addBindValue(id);
    if (!query.exec()) {
        return Result<void>::fail(query.lastError().text());
    }
    return Result<void>::ok();
}

Result<void> SqliteAppRepository::record_usage(qint64 id)
{
    const qint64 now = QDateTime::currentSecsSinceEpoch();
    QSqlQuery query(db_);
    query.prepare(QStringLiteral(
        "INSERT INTO usage_stats (entity_type, entity_id, launch_count, last_used_at) "
        "VALUES ('app', ?, 1, ?) "
        "ON CONFLICT(entity_type, entity_id) DO UPDATE SET "
        "launch_count = launch_count + 1, last_used_at = excluded.last_used_at"));
    query.addBindValue(id);
    query.addBindValue(now);
    if (!query.exec()) {
        return Result<void>::fail(query.lastError().text());
    }
    return Result<void>::ok();
}

Result<void> SqliteAppRepository::clear_catalog()
{
    QSqlQuery query(db_);
    if (!query.exec(QStringLiteral("DELETE FROM apps"))) {
        return Result<void>::fail(query.lastError().text());
    }
    return Result<void>::ok();
}

Result<int> SqliteAppRepository::count()
{
    QSqlQuery query(db_);
    if (!query.exec(QStringLiteral("SELECT COUNT(*) FROM apps")) || !query.next()) {
        return Result<int>::fail(query.lastError().text());
    }
    return Result<int>::ok(query.value(0).toInt());
}

} // namespace quickdeck
