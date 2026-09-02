#include "ui/launcher_controller.h"

#include "services/logger.h"
#include "services/search_service.h"

#include <QApplication>
#include <QClipboard>
#include <QKeySequence>
#include <QSet>

namespace quickdeck {

namespace {

QList<AppEntry> merge_pinned_and_recent(const QList<AppEntry> &pinned,
                                        const QList<AppEntry> &recent,
                                        int limit)
{
    QList<AppEntry> merged;
    QSet<qint64> seen;
    for (const AppEntry &entry : pinned) {
        merged.append(entry);
        seen.insert(entry.id);
        if (merged.size() >= limit) {
            return merged;
        }
    }
    for (const AppEntry &entry : recent) {
        if (seen.contains(entry.id)) {
            continue;
        }
        merged.append(entry);
        seen.insert(entry.id);
        if (merged.size() >= limit) {
            break;
        }
    }
    return merged;
}

} // namespace

LauncherController::LauncherController(ApplicationContext &context, QObject *parent)
    : QObject(parent)
    , context_(context)
{
    load_runtime_settings();
}

void LauncherController::load_runtime_settings()
{
    const Result<bool> close_on_blur =
        context_.settings().get_bool(QStringLiteral("general.close_on_blur"), true);
    if (close_on_blur.is_ok() && close_on_blur_ != close_on_blur.value()) {
        close_on_blur_ = close_on_blur.value();
        emit closeOnBlurChanged();
    }
}

void LauncherController::setQuery(const QString &query)
{
    if (query_ == query) {
        return;
    }
    query_ = query;
    emit queryChanged();
    set_selected_index(0);
    refresh_results();
}

void LauncherController::set_selected_index(int index)
{
    const int clamped = item_count_ > 0 ? qBound(0, index, item_count_ - 1) : 0;
    if (selected_index_ == clamped) {
        return;
    }
    selected_index_ = clamped;
    emit selectedIndexChanged();
}

void LauncherController::show_search()
{
    mode_ = LauncherMode::Search;
    query_.clear();
    visible_ = true;
    selected_index_ = 0;
    emit modeChanged();
    emit visibleChanged();
    emit queryChanged();
    emit selectedIndexChanged();
    refresh_results();
}

void LauncherController::show_clipboard()
{
    mode_ = LauncherMode::Clipboard;
    query_.clear();
    visible_ = true;
    selected_index_ = 0;
    emit modeChanged();
    emit visibleChanged();
    emit queryChanged();
    emit selectedIndexChanged();
    refresh_results();
}

void LauncherController::hide()
{
    confirm_hide();
}

void LauncherController::dismiss()
{
    if (!visible_) {
        return;
    }
    emit hideRequested();
}

void LauncherController::confirm_hide()
{
    if (!visible_) {
        return;
    }
    visible_ = false;
    emit visibleChanged();
}

void LauncherController::move_selection(int delta)
{
    if (item_count_ <= 0 || delta == 0) {
        return;
    }
    int next = selected_index_ + delta;
    if (next < 0) {
        next = item_count_ - 1;
    } else if (next >= item_count_) {
        next = 0;
    }
    set_selected_index(next);
}

void LauncherController::toggle_pin_at(int index)
{
    if (mode_ == LauncherMode::Search) {
        if (index < 0 || index >= app_results_.size()) {
            return;
        }

        const AppEntry &entry = app_results_.at(index);
        if (entry.id <= 0) {
            return;
        }

        const Result<void> pin_result =
            context_.database().apps().set_pinned(entry.id, !entry.is_pinned);
        if (pin_result.is_err()) {
            QD_LOG_WARN(pin_result.error());
            return;
        }

        const qint64 entry_id = entry.id;
        refresh_results();

        int restored_index = 0;
        for (int i = 0; i < app_results_.size(); ++i) {
            if (app_results_.at(i).id == entry_id) {
                restored_index = i;
                break;
            }
        }
        set_selected_index(restored_index);
        return;
    }

    if (index < 0 || index >= clipboard_results_.size()) {
        return;
    }

    const ClipboardEntry &entry = clipboard_results_.at(index);
    if (entry.id <= 0) {
        return;
    }

    const Result<void> pin_result =
        context_.database().clipboard().set_pinned(entry.id, !entry.is_pinned);
    if (pin_result.is_err()) {
        QD_LOG_WARN(pin_result.error());
        return;
    }

    const qint64 entry_id = entry.id;
    refresh_results();

    int restored_index = 0;
    for (int i = 0; i < clipboard_results_.size(); ++i) {
        if (clipboard_results_.at(i).id == entry_id) {
            restored_index = i;
            break;
        }
    }
    set_selected_index(restored_index);
}

void LauncherController::refresh_results()
{
    if (mode_ == LauncherMode::Search) {
        if (query_.trimmed().isEmpty()) {
            const Result<QList<AppEntry>> pinned = context_.database().apps().list_pinned();
            const Result<QList<AppEntry>> recent = context_.database().apps().list_recent(10);
            const QList<AppEntry> pinned_entries = pinned.is_ok() ? pinned.value() : QList<AppEntry>{};
            const QList<AppEntry> recent_entries = recent.is_ok() ? recent.value() : QList<AppEntry>{};
            app_results_ = merge_pinned_and_recent(pinned_entries, recent_entries, 10);
        } else if (PathResolver::looks_like_path(query_)) {
            app_results_.clear();
        } else {
            const Result<QList<AppEntry>> results =
                context_.search_service().search_apps(query_, 10);
            app_results_ = results.is_ok() ? results.value() : QList<AppEntry>{};
        }
        app_model_.set_entries(app_results_);
        update_item_count();
        return;
    }

    const Result<QList<ClipboardEntry>> results =
        context_.search_service().search_clipboard(query_, 50);
    clipboard_results_ = results.is_ok() ? results.value() : QList<ClipboardEntry>{};
    clipboard_model_.set_entries(clipboard_results_);
    update_item_count();
}

void LauncherController::update_item_count()
{
    const int count =
        mode_ == LauncherMode::Search ? app_model_.rowCount() : clipboard_model_.rowCount();
    if (item_count_ == count) {
        return;
    }
    item_count_ = count;
    emit itemCountChanged();
    if (selected_index_ >= item_count_) {
        set_selected_index(item_count_ > 0 ? item_count_ - 1 : 0);
    }
}

void LauncherController::activate_selected(int index)
{
    if (mode_ == LauncherMode::Search) {
        if (index >= 0 && index < app_results_.size()) {
            const AppEntry &selected = app_results_.at(index);
            const Result<void> launch_result = context_.platform().launch_app(selected);
            if (launch_result.is_err()) {
                emit launchFailed(selected.name, launch_result.error());
                return;
            }
            if (selected.id > 0) {
                context_.database().apps().record_usage(selected.id);
            }
            dismiss();
            return;
        }
        if (PathResolver::looks_like_path(query_)) {
            const Result<QString> resolved = PathResolver::resolve(query_);
            if (resolved.is_ok()) {
                context_.platform().open_path(resolved.value());
                dismiss();
            }
        }
        return;
    }

    if (index < 0 || index >= clipboard_results_.size()) {
        return;
    }

    context_.clipboard_monitor().set_suppress_next_change(true);
    QApplication::clipboard()->setText(clipboard_results_.at(index).content);
    dismiss();
}

void LauncherController::reload_settings()
{
    load_runtime_settings();
    setup_hotkeys();
}

void LauncherController::register_hotkeys()
{
    setup_hotkeys();
}

void LauncherController::setup_hotkeys()
{
    context_.platform().unregister_hotkey(QStringLiteral("launcher"));
    context_.platform().unregister_hotkey(QStringLiteral("clipboard"));

    const Result<QString> launcher_hotkey =
        context_.settings().get_string(QStringLiteral("launcher.hotkey"), QStringLiteral("Alt+Space"));
    const Result<QString> clipboard_hotkey = context_.settings().get_string(
        QStringLiteral("clipboard.hotkey"), QStringLiteral("Ctrl+Shift+V"));

    if (launcher_hotkey.is_ok()) {
        const Result<void> result = context_.platform().register_hotkey(
            QStringLiteral("launcher"), QKeySequence(launcher_hotkey.value()),
            [this]() { show_search(); });
        if (result.is_err()) {
            QD_LOG_WARN(result.error());
        }
    }
    if (clipboard_hotkey.is_ok()) {
        const Result<void> result = context_.platform().register_hotkey(
            QStringLiteral("clipboard"), QKeySequence(clipboard_hotkey.value()),
            [this]() { show_clipboard(); });
        if (result.is_err()) {
            QD_LOG_WARN(result.error());
        }
    }
}

} // namespace quickdeck
