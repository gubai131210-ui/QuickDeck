#include "app/application.h"

#include "services/logger.h"
#include "ui/widgets/first_run_wizard.h"

#include <QApplication>
#include <QCoreApplication>
#include <QQmlContext>
#include <QQuickStyle>
#include <QQuickWindow>

namespace quickdeck {

Application::Application(QObject *parent)
    : QObject(parent)
{
}

Result<void> Application::initialize()
{
    QCoreApplication::setApplicationName(QStringLiteral("QuickDeck"));
    QCoreApplication::setOrganizationName(QStringLiteral("QuickDeck"));
    QCoreApplication::setApplicationVersion(QStringLiteral("0.3.0"));

    const Result<void> init_result = context_.initialize();
    if (init_result.is_err()) {
        return init_result;
    }

    const Result<void> locale_result = locale_.apply_saved_language(context_.settings());
    if (locale_result.is_err()) {
        QD_LOG_WARN(locale_result.error());
    }

    launcher_ = std::make_unique<LauncherController>(context_);
    settings_ = std::make_unique<SettingsWindow>(context_, *launcher_, locale_, qml_engine_);
    tray_ = std::make_unique<TrayManager>(context_, *launcher_, *settings_);

    connect(&locale_, &LocaleService::language_changed, tray_.get(), &TrayManager::retranslate_ui);
    connect(&locale_, &LocaleService::language_changed, settings_.get(), &SettingsWindow::retranslate_ui);
    connect(launcher_.get(), &LauncherController::launchFailed, tray_.get(),
            &TrayManager::show_launch_failed);

    const Result<void> tray_result = tray_->initialize();
    if (tray_result.is_err()) {
        return tray_result;
    }

    QQuickStyle::setStyle(QStringLiteral("Basic"));

    qml_engine_.rootContext()->setContextProperty(QStringLiteral("launcher"), launcher_.get());
    qml_engine_.loadFromModule(QStringLiteral("QuickDeckLauncher"), QStringLiteral("LauncherOverlay"));

    if (qml_engine_.rootObjects().isEmpty()) {
        return Result<void>::fail(QStringLiteral("Failed to load launcher overlay QML"));
    }

    overlay_window_ = qobject_cast<QQuickWindow *>(qml_engine_.rootObjects().first());
    connect(launcher_.get(), &LauncherController::visibleChanged, this, [this]() {
        if (overlay_window_ != nullptr && launcher_->visible()) {
            overlay_window_->requestActivate();
        }
    });

    const Result<bool> setup_completed =
        context_.settings().get_bool(QStringLiteral("setup.completed"), false);
    if (setup_completed.is_ok() && !setup_completed.value()) {
        FirstRunWizard wizard(context_, *launcher_);
        const bool completed = wizard.run();
        Q_UNUSED(completed)
    } else {
        launcher_->reload_settings();
    }

    const Result<int> index_result = context_.app_indexer().refresh_catalog();
    if (index_result.is_err()) {
        QD_LOG_WARN(index_result.error());
    }
    context_.clipboard_monitor().start();

    QD_LOG_INFO(QStringLiteral("QuickDeck started (Phase 3)"));
    return Result<void>::ok();
}

int Application::run()
{
    return QApplication::exec();
}

} // namespace quickdeck
