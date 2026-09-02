#include "services/app_indexer.h"

#include "services/logger.h"

#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QSet>
#include <QSettings>
#include <QStandardPaths>

#ifdef Q_OS_WIN
#include <QDirIterator>
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

    QSet<QString> seen_paths;
    for (const QString &root : start_menu_roots) {
        QDirIterator it(root, {QStringLiteral("*.lnk")}, QDir::Files,
                        QDirIterator::Subdirectories);
        while (it.hasNext()) {
            const QString link_path = it.next();
            QFileInfo link_info(link_path);
            AppEntry entry;
            entry.name = link_info.completeBaseName();
            entry.executable_path = link_path;
            entry.platform_id = QStringLiteral("win");
            entry.indexed_at = now;
            entry.created_at = now;
            entry.updated_at = now;
            entry.source_mtime = link_info.lastModified().toSecsSinceEpoch();

            if (seen_paths.contains(entry.executable_path)) {
                continue;
            }
            seen_paths.insert(entry.executable_path);
            entries.append(entry);
        }
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
        emit indexing_failed(scan_result.error());
        return Result<int>::fail(scan_result.error());
    }

    const Result<void> upsert_result = apps_.upsert_batch(scan_result.value());
    if (upsert_result.is_err()) {
        emit indexing_failed(upsert_result.error());
        return Result<int>::fail(upsert_result.error());
    }

    settings_.set_int(QStringLiteral("indexer.last_refresh_at"),
                      static_cast<int>(QDateTime::currentSecsSinceEpoch()));

    const int count = scan_result.value().size();
    QD_LOG_INFO(QStringLiteral("Indexed %1 applications").arg(count));
    emit indexing_finished(count);
    return Result<int>::ok(count);
}

} // namespace quickdeck
