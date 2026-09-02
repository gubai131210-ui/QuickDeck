#pragma once

#include "app/application_context.h"
#include "services/locale_service.h"

#include <QObject>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <memory>

#include "ui/launcher_controller.h"
#include "ui/settings_controller.h"
#include "ui/widgets/tray_manager.h"

namespace quickdeck {

class Application : public QObject {
    Q_OBJECT

public:
    explicit Application(QObject *parent = nullptr);

    [[nodiscard]] Result<void> initialize();
    [[nodiscard]] int run();

private:
    ApplicationContext context_;
    LocaleService locale_;
    std::unique_ptr<LauncherController> launcher_;
    std::unique_ptr<SettingsController> settings_;
    std::unique_ptr<TrayManager> tray_;
    QQmlApplicationEngine qml_engine_;
    QQuickWindow *overlay_window_ = nullptr;
    QQuickWindow *settings_window_ = nullptr;
};

} // namespace quickdeck
