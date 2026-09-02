#pragma once

#include "core/interfaces/repositories.h"
#include "core/result.h"

#include <QObject>
#include <QSqlDatabase>
#include <QString>
#include <memory>

namespace quickdeck {

class DatabaseManager : public QObject {
    Q_OBJECT

public:
    explicit DatabaseManager(QObject *parent = nullptr);
    ~DatabaseManager() override;

    [[nodiscard]] Result<void> open(const QString &database_path,
                                  const QString &migrations_dir);
    [[nodiscard]] QSqlDatabase database() const;

    [[nodiscard]] IAppRepository &apps();
    [[nodiscard]] IClipboardRepository &clipboard();
    [[nodiscard]] ISettingsStore &settings();

    [[nodiscard]] static QString default_database_path();

private:
    QString connection_name_;
    QSqlDatabase db_;
    std::unique_ptr<IAppRepository> app_repo_;
    std::unique_ptr<IClipboardRepository> clipboard_repo_;
    std::unique_ptr<ISettingsStore> settings_repo_;
};

} // namespace quickdeck
