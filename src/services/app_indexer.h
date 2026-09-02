#pragma once

#include "core/interfaces/repositories.h"
#include "core/result.h"

#include <QObject>

namespace quickdeck {

class AppIndexer : public QObject {
    Q_OBJECT

public:
    explicit AppIndexer(IAppRepository &apps, ISettingsStore &settings, QObject *parent = nullptr);

    [[nodiscard]] Result<int> refresh_catalog();

signals:
    void indexing_started();
    void indexing_finished(int app_count);
    void indexing_failed(const QString &error);

private:
    [[nodiscard]] Result<QList<AppEntry>> scan_windows_apps();

    IAppRepository &apps_;
    ISettingsStore &settings_;
};

} // namespace quickdeck
