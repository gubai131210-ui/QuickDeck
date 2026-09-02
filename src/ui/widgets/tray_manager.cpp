#include "ui/widgets/tray_manager.h"

#include "core/user_messages.h"
#include "ui/launcher_controller.h"
#include "ui/settings_controller.h"

#include <QAction>
#include <QApplication>
#include <QMenu>

namespace quickdeck {

TrayManager::TrayManager(ApplicationContext &context,
                         LauncherController &launcher,
                         SettingsController &settings,
                         QObject *parent)
    : QObject(parent)
    , context_(context)
    , launcher_(launcher)
    , settings_(settings)
{
}

Result<void> TrayManager::initialize()
{
    tray_.setIcon(QIcon(QStringLiteral(":/icons/quickdeck.svg")));
    build_menu();
    retranslate_ui();
    tray_.show();
    return Result<void>::ok();
}

void TrayManager::build_menu()
{
    menu_ = new QMenu();

    search_action_ = menu_->addAction(QString());
    connect(search_action_, &QAction::triggered, &launcher_, &LauncherController::show_search);

    clipboard_action_ = menu_->addAction(QString());
    connect(clipboard_action_, &QAction::triggered, &launcher_, &LauncherController::show_clipboard);

    menu_->addSeparator();

    settings_action_ = menu_->addAction(QString());
    connect(settings_action_, &QAction::triggered, &settings_, &SettingsController::show);

    menu_->addSeparator();

    quit_action_ = menu_->addAction(QString());
    connect(quit_action_, &QAction::triggered, qApp, &QApplication::quit);

    tray_.setContextMenu(menu_);
}

void TrayManager::retranslate_ui()
{
    tray_.setToolTip(tr("QuickDeck"));
    if (search_action_ != nullptr) {
        search_action_->setText(tr("Open Search"));
    }
    if (clipboard_action_ != nullptr) {
        clipboard_action_->setText(tr("Open Clipboard"));
    }
    if (settings_action_ != nullptr) {
        settings_action_->setText(tr("Settings"));
    }
    if (quit_action_ != nullptr) {
        quit_action_->setText(tr("Quit"));
    }
}

void TrayManager::show_launch_failed(const QString &app_name, const QString &error_message)
{
    const QString detail = UserMessages::is_error_code(error_message)
                               ? UserMessages::translate_error(error_message)
                               : error_message;
    tray_.showMessage(tr("Launch Failed"),
                      tr("Could not launch %1: %2").arg(app_name, detail),
                      QSystemTrayIcon::Warning,
                      5000);
}

void TrayManager::show_index_refresh_success(int app_count)
{
    const QString body = app_count <= 0 ? tr("No applications were indexed.")
                                        : tr("Indexed %1 applications").arg(app_count);
    tray_.showMessage(tr("Index Refresh"), body, QSystemTrayIcon::Information, 4000);
}

void TrayManager::show_index_refresh_failed(const QString &error_code)
{
    const QString detail = UserMessages::translate_error(error_code);
    tray_.showMessage(tr("Index Refresh"), detail, QSystemTrayIcon::Warning, 5000);
}

void TrayManager::show_quick_paste_failed(const QString &error_code)
{
    tray_.showMessage(tr("Quick Paste"),
                      UserMessages::translate_error(error_code),
                      QSystemTrayIcon::Warning,
                      4000);
}

void TrayManager::show_hotkey_registration_failed(const QString &hotkey_name, const QString &detail)
{
    tray_.showMessage(tr("Hotkey Registration"),
                      tr("Could not register %1: %2").arg(hotkey_name, detail),
                      QSystemTrayIcon::Warning,
                      5000);
}

void TrayManager::show_entries_trimmed(int removed_count)
{
    if (removed_count <= 0) {
        return;
    }
    tray_.showMessage(tr("Clipboard History"),
                      tr("Removed %1 old entries to stay within the limit.").arg(removed_count),
                      QSystemTrayIcon::Information,
                      3000);
}

} // namespace quickdeck
