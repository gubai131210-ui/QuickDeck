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
    tray_.setToolTip(tr("QuickDeck"));
    build_menu();
    tray_.show();
    return Result<void>::ok();
}

void TrayManager::build_menu()
{
    auto *menu = new QMenu();

    auto *search_action = menu->addAction(tr("Open Search"));
    connect(search_action, &QAction::triggered, &launcher_, &LauncherController::show_search);

    auto *clipboard_action = menu->addAction(tr("Open Clipboard"));
    connect(clipboard_action, &QAction::triggered, &launcher_, &LauncherController::show_clipboard);

    menu->addSeparator();

    auto *settings_action = menu->addAction(tr("Settings"));
    connect(settings_action, &QAction::triggered, &settings_, &SettingsWindow::show);

    menu->addSeparator();

    auto *quit_action = menu->addAction(tr("Quit"));
    connect(quit_action, &QAction::triggered, qApp, &QApplication::quit);

    tray_.setContextMenu(menu);
}

} // namespace quickdeck
