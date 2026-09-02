#include "services/app_scanner.h"

#include <QDateTime>
#include <QDirIterator>
#include <QFileInfo>
#include <QSet>

namespace quickdeck {

Result<QList<AppEntry>> scan_lnk_roots(const QStringList &roots)
{
    QList<AppEntry> entries;
    const qint64 now = QDateTime::currentSecsSinceEpoch();
    QSet<QString> seen_paths;

    for (const QString &root : roots) {
        QDirIterator it(root, {QStringLiteral("*.lnk")}, QDir::Files, QDirIterator::Subdirectories);
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

    return Result<QList<AppEntry>>::ok(entries);
}

} // namespace quickdeck
