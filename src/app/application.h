#pragma once

#include "app/application_context.h"
#include "services/locale_service.h"
#include "ui/launcher_controller.h"
#include "ui/widgets/settings_window.h"
#include "ui/widgets/tray_manager.h"

#include <QObject>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <memory>

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
    std::unique_ptr<SettingsWindow> settings_;
    std::unique_ptr<TrayManager> tray_;
    QQmlApplicationEngine qml_engine_;
    QQuickWindow *overlay_window_ = nullptr;
};

} // namespace quickdeck
