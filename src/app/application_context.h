#pragma once

#include "core/domain/types.h"
#include "core/interfaces/i_platform_services.h"
#include "core/interfaces/repositories.h"
#include "data/database_manager.h"
#include "services/app_indexer.h"
#include "services/clipboard_monitor.h"
#include "services/search_service.h"

#include <memory>

class QQmlApplicationEngine;

namespace quickdeck {

class ApplicationContext {
public:
    ApplicationContext();
    ~ApplicationContext();

    [[nodiscard]] Result<void> initialize();

    [[nodiscard]] DatabaseManager &database();
    [[nodiscard]] IPlatformServices &platform();
    [[nodiscard]] AppIndexer &app_indexer();
    [[nodiscard]] ClipboardMonitor &clipboard_monitor();
    [[nodiscard]] SearchService &search_service();
    [[nodiscard]] ISettingsStore &settings();

private:
    std::unique_ptr<DatabaseManager> database_;
    std::unique_ptr<IPlatformServices> platform_;
    std::unique_ptr<AppIndexer> app_indexer_;
    std::unique_ptr<ClipboardMonitor> clipboard_monitor_;
    std::unique_ptr<SearchService> search_service_;
};

} // namespace quickdeck
