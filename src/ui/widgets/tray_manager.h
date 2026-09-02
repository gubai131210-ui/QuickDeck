#pragma once

#include "app/application_context.h"
#include "core/result.h"

#include <QObject>
#include <QSystemTrayIcon>

namespace quickdeck {

class LauncherController;
class SettingsWindow;

class TrayManager : public QObject {
    Q_OBJECT

public:
    TrayManager(ApplicationContext &context, LauncherController &launcher,
                SettingsWindow &settings, QObject *parent = nullptr);

    Result<void> initialize();

private:
    void build_menu();

    ApplicationContext &context_;
    LauncherController &launcher_;
    SettingsWindow &settings_;
    QSystemTrayIcon tray_;
};

} // namespace quickdeck
