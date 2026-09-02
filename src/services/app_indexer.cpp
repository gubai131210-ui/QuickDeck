#include "services/app_indexer.h"

#include "services/app_scanner.h"
#include "services/icon_cache.h"
#include "services/logger.h"

#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QSet>
#include <QSettings>
#include <QStandardPaths>

#ifdef Q_OS_WIN
#include <QSettings>
#endif

namespace quickdeck {

AppIndexer::AppIndexer(IAppRepository &apps, ISettingsStore &settings, QObject *parent)
    : QObject(parent)
    , apps_(apps)
    , settings_(settings)
{
}

Result<QList<AppEntry>> AppIndexer::scan_windows_apps()
{
    QList<AppEntry> entries;
    const qint64 now = QDateTime::currentSecsSinceEpoch();

#ifdef Q_OS_WIN
    const QStringList start_menu_roots = {
        QDir::fromNativeSeparators(QStringLiteral("C:/ProgramData/Microsoft/Windows/Start Menu/Programs")),
        QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation),
    };

    const Result<QList<AppEntry>> lnk_entries = scan_lnk_roots(start_menu_roots);
    if (lnk_entries.is_err()) {
        return lnk_entries;
    }
    entries = lnk_entries.value();

    QSet<QString> seen_paths;
    for (const AppEntry &entry : entries) {
        seen_paths.insert(entry.executable_path);
    }

    QSettings uninstall(
        QStringLiteral("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall"),
        QSettings::NativeFormat);
    for (const QString &key : uninstall.childGroups()) {
        uninstall.beginGroup(key);
        const QString display_name = uninstall.value(QStringLiteral("DisplayName")).toString();
        const QString install_location =
            uninstall.value(QStringLiteral("InstallLocation")).toString();
        const QString display_icon = uninstall.value(QStringLiteral("DisplayIcon")).toString();
        uninstall.endGroup();

        if (display_name.isEmpty()) {
            continue;
        }

        QString executable_path = display_icon;
        const int comma_index = executable_path.indexOf(QLatin1Char(','));
        if (comma_index >= 0) {
            executable_path = executable_path.left(comma_index);
        }
        if (executable_path.isEmpty() && !install_location.isEmpty()) {
            executable_path = QDir(install_location).filePath(display_name + QStringLiteral(".exe"));
        }
        if (executable_path.isEmpty() || seen_paths.contains(executable_path)) {
            continue;
        }

        AppEntry entry;
        entry.name = display_name;
        entry.executable_path = QDir::toNativeSeparators(executable_path);
        entry.platform_id = QStringLiteral("win");
        entry.indexed_at = now;
        entry.created_at = now;
        entry.updated_at = now;
        if (QFileInfo::exists(entry.executable_path)) {
            entry.source_mtime =
                QFileInfo(entry.executable_path).lastModified().toSecsSinceEpoch();
        }
        seen_paths.insert(entry.executable_path);
        entries.append(entry);
    }
#endif

    return Result<QList<AppEntry>>::ok(entries);
}

Result<int> AppIndexer::refresh_catalog()
{
    emit indexing_started();

    const Result<QList<AppEntry>> scan_result = scan_windows_apps();
    if (scan_result.is_err()) {
        QD_LOG_WARN(scan_result.error());
        emit indexing_failed(QStringLiteral("scan_failed"));
        return Result<int>::fail(QStringLiteral("scan_failed"));
    }

    QList<AppEntry> enriched;
    enriched.reserve(scan_result.value().size());
    for (AppEntry entry : scan_result.value()) {
        const Result<QString> icon_path = IconCache::cache_icon_for_entry(entry);
        if (icon_path.is_ok()) {
            entry.icon_cache_path = icon_path.value();
        }
        enriched.append(entry);
    }

    const Result<void> upsert_result = apps_.upsert_batch(enriched);
    if (upsert_result.is_err()) {
        QD_LOG_WARN(upsert_result.error());
        emit indexing_failed(QStringLiteral("upsert_failed"));
        return Result<int>::fail(QStringLiteral("upsert_failed"));
    }

    settings_.set_int(QStringLiteral("indexer.last_refresh_at"),
                      static_cast<int>(QDateTime::currentSecsSinceEpoch()));

    const int count = scan_result.value().size();
    QD_LOG_INFO(QStringLiteral("Indexed %1 applications").arg(count));
    emit indexing_finished(count);
    return Result<int>::ok(count);
}

} // namespace quickdeck
