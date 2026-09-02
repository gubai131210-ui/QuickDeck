#include "ui/launcher_controller.h"

#include "services/search_service.h"

#include <QApplication>
#include <QClipboard>
#include <QKeySequence>

namespace quickdeck {

LauncherController::LauncherController(ApplicationContext &context, QObject *parent)
    : QObject(parent)
    , context_(context)
{
}

void LauncherController::setQuery(const QString &query)
{
    if (query_ == query) {
        return;
    }
    query_ = query;
    emit queryChanged();
    refresh_results();
}

void LauncherController::show_search()
{
    mode_ = LauncherMode::Search;
    query_.clear();
    visible_ = true;
    emit modeChanged();
    emit visibleChanged();
    emit queryChanged();
    refresh_results();
}

void LauncherController::show_clipboard()
{
    mode_ = LauncherMode::Clipboard;
    query_.clear();
    visible_ = true;
    emit modeChanged();
    emit visibleChanged();
    emit queryChanged();
    refresh_results();
}

void LauncherController::hide()
{
    if (!visible_) {
        return;
    }
    visible_ = false;
    emit visibleChanged();
}

void LauncherController::refresh_results()
{
    if (mode_ == LauncherMode::Search) {
        if (query_.trimmed().isEmpty()) {
            const Result<QList<AppEntry>> recent = context_.database().apps().list_recent(5);
            app_results_ = recent.is_ok() ? recent.value() : QList<AppEntry>{};
        } else if (PathResolver::looks_like_path(query_)) {
            app_results_.clear();
        } else {
            const Result<QList<AppEntry>> results =
                context_.search_service().search_apps(query_, 10);
            app_results_ = results.is_ok() ? results.value() : QList<AppEntry>{};
        }
        app_model_.set_entries(app_results_);
        return;
    }

    const Result<QList<ClipboardEntry>> results =
        context_.search_service().search_clipboard(query_, 50);
    clipboard_results_ = results.is_ok() ? results.value() : QList<ClipboardEntry>{};
    clipboard_model_.set_entries(clipboard_results_);
}

void LauncherController::activate_selected(int index)
{
    if (mode_ == LauncherMode::Search) {
        if (index >= 0 && index < app_results_.size()) {
            const AppEntry &selected = app_results_.at(index);
            context_.platform().launch_app(selected);
            if (selected.id > 0) {
                context_.database().apps().record_usage(selected.id);
            }
            hide();
            return;
        }
        if (PathResolver::looks_like_path(query_)) {
            const Result<QString> resolved = PathResolver::resolve(query_);
            if (resolved.is_ok()) {
                context_.platform().open_path(resolved.value());
                hide();
            }
        }
        return;
    }

    if (index < 0 || index >= clipboard_results_.size()) {
        return;
    }

    context_.clipboard_monitor().set_suppress_next_change(true);
    QApplication::clipboard()->setText(clipboard_results_.at(index).content);
    hide();
}

void LauncherController::register_hotkeys()
{
    setup_hotkeys();
}

void LauncherController::setup_hotkeys()
{
    const Result<QString> launcher_hotkey =
        context_.settings().get_string(QStringLiteral("launcher.hotkey"), QStringLiteral("Alt+Space"));
    const Result<QString> clipboard_hotkey = context_.settings().get_string(
        QStringLiteral("clipboard.hotkey"), QStringLiteral("Ctrl+Shift+V"));

    if (launcher_hotkey.is_ok()) {
        context_.platform().register_hotkey(
            QStringLiteral("launcher"), QKeySequence(launcher_hotkey.value()),
            [this]() { show_search(); });
    }
    if (clipboard_hotkey.is_ok()) {
        context_.platform().register_hotkey(
            QStringLiteral("clipboard"), QKeySequence(clipboard_hotkey.value()),
            [this]() { show_clipboard(); });
    }
}

} // namespace quickdeck
