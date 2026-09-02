#pragma once

#include "core/interfaces/repositories.h"

#include <QSqlDatabase>

namespace quickdeck {

class SqliteSettingsStore final : public ISettingsStore {
public:
    explicit SqliteSettingsStore(QSqlDatabase db);

    Result<QString> get_string(const QString &key, const QString &default_value) override;
    Result<int> get_int(const QString &key, int default_value) override;
    Result<bool> get_bool(const QString &key, bool default_value) override;

    Result<void> set_string(const QString &key, const QString &value) override;
    Result<void> set_int(const QString &key, int value) override;
    Result<void> set_bool(const QString &key, bool value) override;

    Result<void> reset_to_defaults() override;

private:
    Result<void> upsert(const QString &key, const QString &value, const QString &value_type);

    QSqlDatabase db_;
};

} // namespace quickdeck
