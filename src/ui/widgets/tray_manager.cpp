#include "ui/widgets/tray_manager.h"

#include "ui/launcher_controller.h"
#include "ui/widgets/settings_window.h"

#include <QAction>
#include <QApplication>
#include <QMenu>

namespace quickdeck {

TrayManager::TrayManager(ApplicationContext &context,
                         LauncherController &launcher,
                         SettingsWindow &settings,
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
    connect(settings_action_, &QAction::triggered, &settings_, &SettingsWindow::show);

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

} // namespace quickdeck
