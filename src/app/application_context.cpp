#include "app/application_context.h"

#include "platform/platform_factory.h"
#include "services/logger.h"

#include <QCoreApplication>
#include <QDir>

#ifndef QUICKDECK_SOURCE_DIR
#define QUICKDECK_SOURCE_DIR "."
#endif

namespace quickdeck {

ApplicationContext::ApplicationContext() = default;

ApplicationContext::~ApplicationContext() = default;

Result<void> ApplicationContext::initialize()
{
    database_ = std::make_unique<DatabaseManager>();
    platform_ = create_platform_services();

    const QString migration_path = [&]() {
        const QStringList candidates = {
            QCoreApplication::applicationDirPath() + QStringLiteral("/migrations"),
            QStringLiteral(QUICKDECK_SOURCE_DIR "/migrations"),
        };
        for (const QString &candidate : candidates) {
            if (QDir(candidate).exists()) {
                return candidate;
            }
        }
        return candidates.last();
    }();

    const Result<void> open_result =
        database_->open(DatabaseManager::default_database_path(), migration_path);
    if (open_result.is_err()) {
        QD_LOG_ERROR(open_result.error());
        return open_result;
    }

    app_indexer_ = std::make_unique<AppIndexer>(database_->apps(), database_->settings());
    clipboard_monitor_ =
        std::make_unique<ClipboardMonitor>(database_->clipboard(), database_->settings());
    search_service_ = std::make_unique<SearchService>(database_->apps(), database_->clipboard());

    QD_LOG_INFO(QStringLiteral("ApplicationContext initialized"));
    return Result<void>::ok();
}

DatabaseManager &ApplicationContext::database()
{
    return *database_;
}

IPlatformServices &ApplicationContext::platform()
{
    return *platform_;
}

AppIndexer &ApplicationContext::app_indexer()
{
    return *app_indexer_;
}

ClipboardMonitor &ApplicationContext::clipboard_monitor()
{
    return *clipboard_monitor_;
}

SearchService &ApplicationContext::search_service()
{
    return *search_service_;
}

ISettingsStore &ApplicationContext::settings()
{
    return database_->settings();
}

} // namespace quickdeck
