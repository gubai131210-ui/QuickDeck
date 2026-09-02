#pragma once

#include "core/interfaces/repositories.h"

#include <QSqlDatabase>

namespace quickdeck {

class SqliteClipboardRepository final : public IClipboardRepository {
public:
    explicit SqliteClipboardRepository(QSqlDatabase db);

    Result<qint64> insert(const ClipboardEntry &entry) override;
    Result<QList<ClipboardEntry>> list_recent(int limit, int offset) override;
    Result<QList<ClipboardEntry>> search(const QString &query,
                                         const ClipboardSearchOptions &options) override;
    Result<QList<ClipboardEntry>> list_pinned() override;
    Result<void> set_pinned(qint64 id, bool pinned) override;
    Result<void> remove(qint64 id) override;
    Result<void> clear_unpinned() override;
    Result<int> enforce_retention(int max_entries, int max_age_days) override;
    Result<bool> is_duplicate(const QString &content) override;
    Result<int> count() override;

private:
    [[nodiscard]] ClipboardEntry row_to_entry(const QSqlQuery &query) const;
    [[nodiscard]] QString make_preview(const QString &content) const;

    QSqlDatabase db_;
};

} // namespace quickdeck
