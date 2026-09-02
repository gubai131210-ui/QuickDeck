#pragma once

#include "core/domain/types.h"
#include "core/result.h"

#include <QList>
#include <QString>

namespace quickdeck {

struct AppSearchOptions {
    int limit = 10;
    bool include_unpinned_only = false;
};

class IAppRepository {
public:
    virtual ~IAppRepository() = default;

    virtual Result<void> upsert(const AppEntry &entry) = 0;
    virtual Result<void> upsert_batch(const QList<AppEntry> &entries) = 0;
    virtual Result<QList<AppEntry>> search(const QString &query,
                                           const AppSearchOptions &options) = 0;
    virtual Result<QList<AppEntry>> list_recent(int limit) = 0;
    virtual Result<QList<AppEntry>> list_pinned() = 0;
    virtual Result<void> set_pinned(qint64 id, bool pinned) = 0;
    virtual Result<void> record_usage(qint64 id) = 0;
    virtual Result<void> clear_catalog() = 0;
    virtual Result<int> count() = 0;
};

struct ClipboardSearchOptions {
    int limit = 50;
    int offset = 0;
};

class IClipboardRepository {
public:
    virtual ~IClipboardRepository() = default;

    virtual Result<qint64> insert(const ClipboardEntry &entry) = 0;
    virtual Result<QList<ClipboardEntry>> list_recent(int limit, int offset) = 0;
    virtual Result<QList<ClipboardEntry>> search(const QString &query,
                                                 const ClipboardSearchOptions &options) = 0;
    virtual Result<QList<ClipboardEntry>> list_pinned() = 0;
    virtual Result<void> set_pinned(qint64 id, bool pinned) = 0;
    virtual Result<void> remove(qint64 id) = 0;
    virtual Result<void> clear_unpinned() = 0;
    virtual Result<void> enforce_retention(int max_entries, int max_age_days) = 0;
    virtual Result<bool> is_duplicate(const QString &content) = 0;
    virtual Result<int> count() = 0;
};

class ISettingsStore {
public:
    virtual ~ISettingsStore() = default;

    virtual Result<QString> get_string(const QString &key, const QString &default_value) = 0;
    virtual Result<int> get_int(const QString &key, int default_value) = 0;
    virtual Result<bool> get_bool(const QString &key, bool default_value) = 0;

    virtual Result<void> set_string(const QString &key, const QString &value) = 0;
    virtual Result<void> set_int(const QString &key, int value) = 0;
    virtual Result<void> set_bool(const QString &key, bool value) = 0;

    virtual Result<void> reset_to_defaults() = 0;
};

} // namespace quickdeck
