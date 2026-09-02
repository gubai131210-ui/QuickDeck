#include "ui/settings_controller.h"

#include "services/app_indexer.h"
#include "services/logger.h"
#include "ui/launcher_controller.h"

#include <QKeySequence>
#include <QMessageBox>

namespace quickdeck {

SettingsController::SettingsController(ApplicationContext &context,
                                       LauncherController &launcher,
                                       LocaleService &locale,
                                       QQmlApplicationEngine &qml_engine,
                                       QObject *parent)
    : QObject(parent)
    , context_(context)
    , launcher_(launcher)
    , locale_(locale)
    , qml_engine_(qml_engine)
{
}

void SettingsController::set_visible(bool visible)
{
    if (visible_ == visible) {
        return;
    }
    visible_ = visible;
    emit visibleChanged();
}

void SettingsController::set_current_tab(int tab)
{
    const int clamped = qBound(0, tab, 2);
    if (current_tab_ == clamped) {
        return;
    }
    current_tab_ = clamped;
    emit currentTabChanged();
}

void SettingsController::set_language(const QString &language)
{
    if (language_ == language) {
        return;
    }
    language_ = language;
    emit languageChanged();
}

void SettingsController::set_auto_start(bool enabled)
{
    if (auto_start_ == enabled) {
        return;
    }
    auto_start_ = enabled;
    emit autoStartChanged();
}

void SettingsController::set_close_on_blur(bool enabled)
{
    if (close_on_blur_ == enabled) {
        return;
    }
    close_on_blur_ = enabled;
    emit closeOnBlurChanged();
}

void SettingsController::set_debug_log(bool enabled)
{
    if (debug_log_ == enabled) {
        return;
    }
    debug_log_ = enabled;
    emit debugLogChanged();
}

void SettingsController::set_launcher_hotkey(const QString &hotkey)
{
    if (launcher_hotkey_ == hotkey) {
        return;
    }
    launcher_hotkey_ = hotkey;
    emit launcherHotkeyChanged();
}

void SettingsController::set_clipboard_hotkey(const QString &hotkey)
{
    if (clipboard_hotkey_ == hotkey) {
        return;
    }
    clipboard_hotkey_ = hotkey;
    emit clipboardHotkeyChanged();
}

void SettingsController::set_quick_paste_hotkey(const QString &hotkey)
{
    if (quick_paste_hotkey_ == hotkey) {
        return;
    }
    quick_paste_hotkey_ = hotkey;
    emit quickPasteHotkeyChanged();
}

void SettingsController::set_max_entries(int value)
{
    if (max_entries_ == value) {
        return;
    }
    max_entries_ = value;
    emit maxEntriesChanged();
}

void SettingsController::set_max_chars(int value)
{
    if (max_chars_ == value) {
        return;
    }
    max_chars_ = value;
    emit maxCharsChanged();
}

void SettingsController::set_monitoring_enabled(bool enabled)
{
    if (monitoring_enabled_ == enabled) {
        return;
    }
    monitoring_enabled_ = enabled;
    emit monitoringEnabledChanged();
}

void SettingsController::set_simulate_paste_on_activate(bool enabled)
{
    if (simulate_paste_on_activate_ == enabled) {
        return;
    }
    simulate_paste_on_activate_ = enabled;
    emit simulatePasteOnActivateChanged();
}

void SettingsController::show()
{
    load();
    set_visible(true);
}

void SettingsController::hide()
{
    set_visible(false);
}

void SettingsController::load()
{
    ISettingsStore &store = context_.settings();
    set_auto_start(store.get_bool(QStringLiteral("general.auto_start"), false).value());
    set_close_on_blur(store.get_bool(QStringLiteral("general.close_on_blur"), true).value());
    set_debug_log(store.get_bool(QStringLiteral("general.debug_log"), false).value());

    const Result<QString> language =
        store.get_string(QStringLiteral("general.language"), LocaleService::detect_system_language());
    set_language(LocaleService::normalize_language(language.value()));

    set_launcher_hotkey(
        store.get_string(QStringLiteral("launcher.hotkey"), QStringLiteral("Alt+Space")).value());
    set_clipboard_hotkey(
        store.get_string(QStringLiteral("clipboard.hotkey"), QStringLiteral("Ctrl+Shift+V")).value());
    set_quick_paste_hotkey(store.get_string(QStringLiteral("clipboard.quick_paste_hotkey"),
                                            QStringLiteral("Ctrl+Alt+V"))
                               .value());

    set_max_entries(store.get_int(QStringLiteral("clipboard.max_entries"), 200).value());
    set_max_chars(store.get_int(QStringLiteral("clipboard.max_char_length"), 10000).value());
    set_monitoring_enabled(
        store.get_bool(QStringLiteral("clipboard.monitoring_enabled"), true).value());
    set_simulate_paste_on_activate(
        store.get_bool(QStringLiteral("clipboard.simulate_paste_on_activate"), false).value());
    apply_debug_log(debug_log_);
}

bool SettingsController::save()
{
    if (!validate_hotkeys()) {
        return false;
    }

    const QString previous_language = locale_.current_language();
    ISettingsStore &store = context_.settings();

    store.set_bool(QStringLiteral("general.auto_start"), auto_start_);
    store.set_bool(QStringLiteral("general.close_on_blur"), close_on_blur_);
    store.set_bool(QStringLiteral("general.debug_log"), debug_log_);
    store.set_string(QStringLiteral("general.language"), language_);
    store.set_string(QStringLiteral("launcher.hotkey"), launcher_hotkey_);
    store.set_string(QStringLiteral("clipboard.hotkey"), clipboard_hotkey_);
    store.set_string(QStringLiteral("clipboard.quick_paste_hotkey"), quick_paste_hotkey_);
    store.set_int(QStringLiteral("clipboard.max_entries"), max_entries_);
    store.set_int(QStringLiteral("clipboard.max_char_length"), max_chars_);
    store.set_bool(QStringLiteral("clipboard.monitoring_enabled"), monitoring_enabled_);
    store.set_bool(QStringLiteral("clipboard.simulate_paste_on_activate"), simulate_paste_on_activate_);

    apply_debug_log(debug_log_);
    context_.platform().set_auto_start_enabled(auto_start_);

    if (language_ != previous_language) {
        locale_.set_language(store, language_, &qml_engine_);
    }

    launcher_.reload_settings();
    hide();
    return true;
}

void SettingsController::refresh_index()
{
    context_.app_indexer().refresh_catalog(true);
}

QStringList SettingsController::supported_languages() const
{
    return LocaleService::supported_languages();
}

QString SettingsController::language_display_name(const QString &code) const
{
    return LocaleService::display_name(code);
}

bool SettingsController::validate_hotkeys() const
{
    ISettingsStore &store = context_.settings();
    const QString stored_launcher =
        store.get_string(QStringLiteral("launcher.hotkey"), QStringLiteral("Alt+Space")).value();
    const QString stored_clipboard =
        store.get_string(QStringLiteral("clipboard.hotkey"), QStringLiteral("Ctrl+Shift+V"))
            .value();
    const Result<QString> stored_quick_paste =
        store.get_string(QStringLiteral("clipboard.quick_paste_hotkey"), QStringLiteral("Ctrl+Alt+V"));

    const QStringList new_hotkeys = {launcher_hotkey_, clipboard_hotkey_, quick_paste_hotkey_};
    for (int i = 0; i < new_hotkeys.size(); ++i) {
        if (new_hotkeys.at(i).isEmpty()) {
            continue;
        }
        for (int j = i + 1; j < new_hotkeys.size(); ++j) {
            if (!new_hotkeys.at(j).isEmpty() && new_hotkeys.at(i) == new_hotkeys.at(j)) {
                const QMessageBox::StandardButton answer = QMessageBox::warning(
                    nullptr, tr("Duplicate Hotkeys"), tr("Shortcuts must be unique. Save anyway?"),
                    QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
                return answer == QMessageBox::Yes;
            }
        }
    }

    auto needs_probe = [](const QString &stored, const QString &updated) {
        return stored != updated;
    };

    if (needs_probe(stored_launcher, launcher_hotkey_)) {
        const Result<bool> launcher_ok =
            context_.platform().is_hotkey_available(QKeySequence(launcher_hotkey_));
        if (launcher_ok.is_err() || !launcher_ok.value()) {
            const QMessageBox::StandardButton answer = QMessageBox::warning(
                nullptr, tr("Hotkey Conflict"),
                tr("Launcher shortcut appears unavailable. Save anyway?"), QMessageBox::Yes | QMessageBox::No,
                QMessageBox::No);
            if (answer != QMessageBox::Yes) {
                return false;
            }
        }
    }

    if (needs_probe(stored_clipboard, clipboard_hotkey_)) {
        const Result<bool> clipboard_ok =
            context_.platform().is_hotkey_available(QKeySequence(clipboard_hotkey_));
        if (clipboard_ok.is_err() || !clipboard_ok.value()) {
            const QMessageBox::StandardButton answer = QMessageBox::warning(
                nullptr, tr("Hotkey Conflict"),
                tr("Clipboard shortcut appears unavailable. Save anyway?"), QMessageBox::Yes | QMessageBox::No,
                QMessageBox::No);
            if (answer != QMessageBox::Yes) {
                return false;
            }
        }
    }

    if (stored_quick_paste.is_ok() &&
        needs_probe(stored_quick_paste.value(), quick_paste_hotkey_) && !quick_paste_hotkey_.isEmpty()) {
        const Result<bool> quick_paste_ok =
            context_.platform().is_hotkey_available(QKeySequence(quick_paste_hotkey_));
        if (quick_paste_ok.is_err() || !quick_paste_ok.value()) {
            const QMessageBox::StandardButton answer = QMessageBox::warning(
                nullptr, tr("Hotkey Conflict"),
                tr("Quick paste shortcut appears unavailable. Save anyway?"), QMessageBox::Yes | QMessageBox::No,
                QMessageBox::No);
            if (answer != QMessageBox::Yes) {
                return false;
            }
        }
    }

    return true;
}

void SettingsController::apply_debug_log(bool enabled)
{
    Logger::instance().set_debug_enabled(enabled);
}

} // namespace quickdeck
