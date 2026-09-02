#pragma once

#include "core/domain/types.h"
#include "core/interfaces/repositories.h"
#include "core/result.h"

#include <QObject>

namespace quickdeck {

class PathResolver {
public:
    [[nodiscard]] static Result<QString> resolve(const QString &input);
    [[nodiscard]] static bool looks_like_path(const QString &input);
};

class SearchService : public QObject {
    Q_OBJECT

public:
    SearchService(IAppRepository &apps, IClipboardRepository &clipboard, QObject *parent = nullptr);

    [[nodiscard]] Result<QList<AppEntry>> search_apps(const QString &query, int limit = 10);
    [[nodiscard]] Result<QList<ClipboardEntry>> search_clipboard(const QString &query, int limit = 50);

private:
    IAppRepository &apps_;
    IClipboardRepository &clipboard_;
};

} // namespace quickdeck
