#pragma once

#include "app/application_context.h"
#include "core/result.h"
#include "services/locale_service.h"

#include <QObject>
#include <QSystemTrayIcon>

class QAction;
class QMenu;

namespace quickdeck {

class LauncherController;
class SettingsWindow;

class TrayManager : public QObject {
    Q_OBJECT

public:
    TrayManager(ApplicationContext &context, LauncherController &launcher,
                SettingsWindow &settings, QObject *parent = nullptr);

    Result<void> initialize();
    void retranslate_ui();
    void show_launch_failed(const QString &app_name, const QString &error_message);
    void show_index_refresh_success(int app_count);
    void show_index_refresh_failed(const QString &error_code);

private:
    void build_menu();

    ApplicationContext &context_;
    LauncherController &launcher_;
    SettingsWindow &settings_;
    QSystemTrayIcon tray_;
    QMenu *menu_ = nullptr;
    QAction *search_action_ = nullptr;
    QAction *clipboard_action_ = nullptr;
    QAction *settings_action_ = nullptr;
    QAction *quit_action_ = nullptr;
};

} // namespace quickdeck
