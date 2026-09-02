#include "services/search_service.h"

#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>

namespace quickdeck {

bool PathResolver::looks_like_path(const QString &input)
{
    const QString trimmed = input.trimmed();
    if (trimmed.isEmpty()) {
        return false;
    }
    if (trimmed.startsWith(QStringLiteral("~/")) || trimmed.startsWith(QStringLiteral("~\\"))) {
        return true;
    }
#ifdef Q_OS_WIN
    return trimmed.size() >= 3 && trimmed.at(1) == QLatin1Char(':') &&
           (trimmed.at(2) == QLatin1Char('\\') || trimmed.at(2) == QLatin1Char('/'));
#else
    return trimmed.startsWith(QLatin1Char('/'));
#endif
}

Result<QString> PathResolver::resolve(const QString &input)
{
    QString path = input.trimmed();
    if (path.startsWith(QStringLiteral("~"))) {
        path.replace(0, 1, QDir::homePath());
    }
    path = QDir::cleanPath(path);
    if (!QFileInfo::exists(path)) {
        return Result<QString>::fail(
            QStringLiteral("Path does not exist: %1").arg(path));
    }
    return Result<QString>::ok(path);
}

SearchService::SearchService(IAppRepository &apps,
                             IClipboardRepository &clipboard,
                             QObject *parent)
    : QObject(parent)
    , apps_(apps)
    , clipboard_(clipboard)
{
}

Result<QList<AppEntry>> SearchService::search_apps(const QString &query, int limit)
{
    AppSearchOptions options;
    options.limit = limit;
    return apps_.search(query, options);
}

Result<QList<ClipboardEntry>> SearchService::search_clipboard(const QString &query, int limit)
{
    ClipboardSearchOptions options;
    options.limit = limit;
    return clipboard_.search(query, options);
}

} // namespace quickdeck
