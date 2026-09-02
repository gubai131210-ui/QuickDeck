#include "app/application.h"

#include "services/app_indexer.h"
#include "services/clipboard_monitor.h"
#include "services/logger.h"
#include "ui/widgets/first_run_wizard.h"

#include <QApplication>
#include <QCoreApplication>
#include <QQmlContext>
#include <QQuickStyle>
#include <QQuickWindow>
#include <QTimer>

namespace quickdeck {

Application::Application(QObject *parent)
    : QObject(parent)
{
}

Result<void> Application::initialize()
{
    QCoreApplication::setApplicationName(QStringLiteral("QuickDeck"));
    QCoreApplication::setOrganizationName(QStringLiteral("QuickDeck"));
    QCoreApplication::setApplicationVersion(QStringLiteral("0.7.0"));

    const Result<void> init_result = context_.initialize();
    if (init_result.is_err()) {
        return init_result;
    }

    Logger::instance().set_debug_enabled(
        context_.settings().get_bool(QStringLiteral("general.debug_log"), false).value());

    const Result<void> locale_result = locale_.apply_saved_language(context_.settings());
    if (locale_result.is_err()) {
        QD_LOG_WARN(locale_result.error());
    }

    launcher_ = std::make_unique<LauncherController>(context_);
    settings_ = std::make_unique<SettingsController>(context_, *launcher_, locale_, qml_engine_);
    launcher_->set_settings_controller(settings_.get());
    launcher_->set_locale_service(&locale_);
    launcher_->set_qml_engine(&qml_engine_);
    tray_ = std::make_unique<TrayManager>(context_, *launcher_, *settings_);

    connect(&locale_, &LocaleService::language_changed, tray_.get(), &TrayManager::retranslate_ui);
    connect(launcher_.get(), &LauncherController::launchFailed, tray_.get(),
            &TrayManager::show_launch_failed);
    connect(launcher_.get(), &LauncherController::quickPasteFailed, tray_.get(),
            &TrayManager::show_quick_paste_failed);
    connect(launcher_.get(), &LauncherController::hotkeyRegistrationFailed, tray_.get(),
            &TrayManager::show_hotkey_registration_failed);

    const Result<void> tray_result = tray_->initialize();
    if (tray_result.is_err()) {
        return tray_result;
    }

    QQuickStyle::setStyle(QStringLiteral("Basic"));

    qml_engine_.rootContext()->setContextProperty(QStringLiteral("launcher"), launcher_.get());
    qml_engine_.rootContext()->setContextProperty(QStringLiteral("settings"), settings_.get());
    qml_engine_.loadFromModule(QStringLiteral("QuickDeckLauncher"), QStringLiteral("LauncherOverlay"));
    qml_engine_.loadFromModule(QStringLiteral("QuickDeckLauncher"), QStringLiteral("SettingsOverlay"));

    const QList<QObject *> roots = qml_engine_.rootObjects();
    for (QObject *root : roots) {
        auto *window = qobject_cast<QQuickWindow *>(root);
        if (window == nullptr) {
            continue;
        }
        if (window->objectName() == QStringLiteral("settingsOverlay")) {
            settings_window_ = window;
        } else if (window->objectName() == QStringLiteral("launcherOverlay")) {
            overlay_window_ = window;
        }
    }

    if (overlay_window_ == nullptr) {
        return Result<void>::fail(QStringLiteral("Failed to load launcher overlay QML"));
    }

    connect(launcher_.get(), &LauncherController::visibleChanged, this, [this]() {
        if (overlay_window_ != nullptr && launcher_->visible()) {
            overlay_window_->requestActivate();
        }
    });
    connect(settings_.get(), &SettingsController::visibleChanged, this, [this]() {
        if (settings_window_ != nullptr && settings_->visible()) {
            settings_window_->requestActivate();
        }
    });

    const Result<bool> setup_completed =
        context_.settings().get_bool(QStringLiteral("setup.completed"), false);
    if (setup_completed.is_ok() && !setup_completed.value()) {
        FirstRunWizard wizard(context_, *launcher_, locale_, qml_engine_);
        const bool completed = wizard.run();
        Q_UNUSED(completed)
    } else {
        launcher_->reload_settings();
    }

    QTimer::singleShot(0, this, [this]() {
        const Result<int> index_result = context_.app_indexer().refresh_catalog();
        if (index_result.is_err()) {
            QD_LOG_WARN(index_result.error());
        }
    });

    connect(&context_.app_indexer(), &AppIndexer::user_indexing_finished, tray_.get(),
            &TrayManager::show_index_refresh_success);
    connect(&context_.app_indexer(), &AppIndexer::user_indexing_failed, tray_.get(),
            &TrayManager::show_index_refresh_failed);
    connect(&context_.clipboard_monitor(), &ClipboardMonitor::entries_trimmed, tray_.get(),
            &TrayManager::show_entries_trimmed);

    context_.clipboard_monitor().start();

    QD_LOG_INFO(QStringLiteral("QuickDeck started"));
    return Result<void>::ok();
}

int Application::run()
{
    return QApplication::exec();
}

} // namespace quickdeck
