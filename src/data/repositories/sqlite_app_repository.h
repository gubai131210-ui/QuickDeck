#pragma once

#include "core/interfaces/repositories.h"

#include <QSqlDatabase>

namespace quickdeck {

class SqliteAppRepository final : public IAppRepository {
public:
    explicit SqliteAppRepository(QSqlDatabase db);

    Result<void> upsert(const AppEntry &entry) override;
    Result<void> upsert_batch(const QList<AppEntry> &entries) override;
    Result<QList<AppEntry>> search(const QString &query,
                                   const AppSearchOptions &options) override;
    Result<QList<AppEntry>> list_recent(int limit) override;
    Result<QList<AppEntry>> list_pinned() override;
    Result<void> set_pinned(qint64 id, bool pinned) override;
    Result<void> record_usage(qint64 id) override;
    Result<void> clear_catalog() override;
    Result<int> count() override;

private:
    [[nodiscard]] AppEntry row_to_entry(const QSqlQuery &query) const;

    QSqlDatabase db_;
};

} // namespace quickdeck
