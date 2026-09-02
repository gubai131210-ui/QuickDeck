#include "services/icon_cache.h"

#include <QDir>
#include <QFileIconProvider>
#include <QFileInfo>
#include <QStandardPaths>
#include <QCryptographicHash>

#ifdef Q_OS_WIN
#include "platform/win/win_lnk_resolver.h"
#endif

namespace quickdeck {

QString IconCache::cache_directory()
{
    const QString base =
        QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    const QString dir = base + QStringLiteral("/QuickDeck/icons");
    QDir().mkpath(dir);
    return dir;
}

namespace {

QString icon_lookup_path(const AppEntry &entry)
{
    QString lookup = entry.executable_path;
#ifdef Q_OS_WIN
    if (lookup.endsWith(QStringLiteral(".lnk"), Qt::CaseInsensitive)) {
        const Result<QString> target = resolve_lnk_target(lookup);
        if (target.is_ok()) {
            lookup = target.value();
        }
    }
#endif
    return lookup;
}

QString cache_file_path(const QString &executable_path)
{
    const QByteArray hash =
        QCryptographicHash::hash(executable_path.toUtf8(), QCryptographicHash::Sha1).toHex();
    return IconCache::cache_directory() + QChar(QLatin1Char('/')) + QString::fromLatin1(hash)
           + QStringLiteral(".png");
}

} // namespace

Result<QString> IconCache::cache_icon_for_entry(const AppEntry &entry)
{
    const QString lookup = icon_lookup_path(entry);
    if (lookup.isEmpty()) {
        return Result<QString>::fail(QStringLiteral("No icon lookup path"));
    }

    const QFileInfo info(lookup);
    if (!info.exists() && !lookup.endsWith(QStringLiteral(".exe"), Qt::CaseInsensitive)) {
        return Result<QString>::fail(QStringLiteral("Icon source missing"));
    }

    const QString output_path = cache_file_path(entry.executable_path);
    const QFileInfo cached(output_path);
    if (cached.exists() && cached.lastModified() >= info.lastModified()) {
        return Result<QString>::ok(output_path);
    }

    QFileIconProvider provider;
    const QIcon icon = provider.icon(info);
    if (icon.isNull()) {
        return Result<QString>::fail(QStringLiteral("No icon for path"));
    }

    const QPixmap pixmap = icon.pixmap(32, 32);
    if (pixmap.isNull() || !pixmap.save(output_path, "PNG")) {
        return Result<QString>::fail(QStringLiteral("Failed to write icon cache"));
    }

    return Result<QString>::ok(output_path);
}

} // namespace quickdeck
