#include "app/application.h"

#include "services/logger.h"

#include <QApplication>
#include <QCoreApplication>
#include <QQmlContext>

namespace quickdeck {

Application::Application(QObject *parent)
    : QObject(parent)
{
}

Result<void> Application::initialize()
{
    QCoreApplication::setApplicationName(QStringLiteral("QuickDeck"));
    QCoreApplication::setOrganizationName(QStringLiteral("QuickDeck"));
    QCoreApplication::setApplicationVersion(QStringLiteral("0.1.0"));

    const Result<void> init_result = context_.initialize();
    if (init_result.is_err()) {
        return init_result;
    }

    launcher_ = std::make_unique<LauncherController>(context_);
    settings_ = std::make_unique<SettingsWindow>(context_);
    tray_ = std::make_unique<TrayManager>(context_, *launcher_, *settings_);

    const Result<void> tray_result = tray_->initialize();
    if (tray_result.is_err()) {
        return tray_result;
    }

    qml_engine_.rootContext()->setContextProperty(QStringLiteral("launcher"), launcher_.get());
    qml_engine_.loadFromModule(QStringLiteral("QuickDeckLauncher"), QStringLiteral("LauncherOverlay"));

    launcher_->register_hotkeys();
    const quickdeck::Result<int> index_result = context_.app_indexer().refresh_catalog();
    if (index_result.is_err()) {
        QD_LOG_WARN(index_result.error());
    }
    context_.clipboard_monitor().start();

    QD_LOG_INFO(QStringLiteral("QuickDeck started"));
    return Result<void>::ok();
}

int Application::run()
{
    return QApplication::exec();
}

} // namespace quickdeck
