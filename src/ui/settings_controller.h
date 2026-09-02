#pragma once

#include "app/application_context.h"
#include "services/locale_service.h"

#include <QObject>
#include <QStringList>

class QQmlApplicationEngine;

namespace quickdeck {

class LauncherController;

class SettingsController : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool visible READ visible WRITE set_visible NOTIFY visibleChanged)
    Q_PROPERTY(int currentTab READ current_tab WRITE set_current_tab NOTIFY currentTabChanged)
    Q_PROPERTY(QString language READ language WRITE set_language NOTIFY languageChanged)
    Q_PROPERTY(bool autoStart READ auto_start WRITE set_auto_start NOTIFY autoStartChanged)
    Q_PROPERTY(bool closeOnBlur READ close_on_blur WRITE set_close_on_blur NOTIFY closeOnBlurChanged)
    Q_PROPERTY(bool debugLog READ debug_log WRITE set_debug_log NOTIFY debugLogChanged)
    Q_PROPERTY(QString launcherHotkey READ launcher_hotkey WRITE set_launcher_hotkey NOTIFY launcherHotkeyChanged)
    Q_PROPERTY(QString clipboardHotkey READ clipboard_hotkey WRITE set_clipboard_hotkey NOTIFY clipboardHotkeyChanged)
    Q_PROPERTY(QString quickPasteHotkey READ quick_paste_hotkey WRITE set_quick_paste_hotkey NOTIFY quickPasteHotkeyChanged)
    Q_PROPERTY(int maxEntries READ max_entries WRITE set_max_entries NOTIFY maxEntriesChanged)
    Q_PROPERTY(int maxChars READ max_chars WRITE set_max_chars NOTIFY maxCharsChanged)
    Q_PROPERTY(bool monitoringEnabled READ monitoring_enabled WRITE set_monitoring_enabled NOTIFY monitoringEnabledChanged)
    Q_PROPERTY(
        bool simulatePasteOnActivate READ simulate_paste_on_activate WRITE set_simulate_paste_on_activate NOTIFY simulatePasteOnActivateChanged)

public:
    SettingsController(ApplicationContext &context, LauncherController &launcher,
                       LocaleService &locale, QQmlApplicationEngine &qml_engine,
                       QObject *parent = nullptr);

    [[nodiscard]] bool visible() const { return visible_; }
    [[nodiscard]] int current_tab() const { return current_tab_; }
    [[nodiscard]] QString language() const { return language_; }
    [[nodiscard]] bool auto_start() const { return auto_start_; }
    [[nodiscard]] bool close_on_blur() const { return close_on_blur_; }
    [[nodiscard]] bool debug_log() const { return debug_log_; }
    [[nodiscard]] QString launcher_hotkey() const { return launcher_hotkey_; }
    [[nodiscard]] QString clipboard_hotkey() const { return clipboard_hotkey_; }
    [[nodiscard]] QString quick_paste_hotkey() const { return quick_paste_hotkey_; }
    [[nodiscard]] int max_entries() const { return max_entries_; }
    [[nodiscard]] int max_chars() const { return max_chars_; }
    [[nodiscard]] bool monitoring_enabled() const { return monitoring_enabled_; }
    [[nodiscard]] bool simulate_paste_on_activate() const { return simulate_paste_on_activate_; }

    void set_visible(bool visible);
    void set_current_tab(int tab);
    void set_language(const QString &language);
    void set_auto_start(bool enabled);
    void set_close_on_blur(bool enabled);
    void set_debug_log(bool enabled);
    void set_launcher_hotkey(const QString &hotkey);
    void set_clipboard_hotkey(const QString &hotkey);
    void set_quick_paste_hotkey(const QString &hotkey);
    void set_max_entries(int value);
    void set_max_chars(int value);
    void set_monitoring_enabled(bool enabled);
    void set_simulate_paste_on_activate(bool enabled);

    Q_INVOKABLE void show();
    Q_INVOKABLE void hide();
    Q_INVOKABLE void load();
    Q_INVOKABLE bool save();
    Q_INVOKABLE void refresh_index();
    Q_INVOKABLE QStringList supported_languages() const;
    Q_INVOKABLE QString language_display_name(const QString &code) const;

signals:
    void visibleChanged();
    void currentTabChanged();
    void languageChanged();
    void autoStartChanged();
    void closeOnBlurChanged();
    void debugLogChanged();
    void launcherHotkeyChanged();
    void clipboardHotkeyChanged();
    void quickPasteHotkeyChanged();
    void maxEntriesChanged();
    void maxCharsChanged();
    void monitoringEnabledChanged();
    void simulatePasteOnActivateChanged();
    void saveFailed(const QString &title, const QString &message);

private:
    [[nodiscard]] bool validate_hotkeys() const;
    void apply_debug_log(bool enabled);

    ApplicationContext &context_;
    LauncherController &launcher_;
    LocaleService &locale_;
    QQmlApplicationEngine &qml_engine_;
    bool visible_ = false;
    int current_tab_ = 0;
    QString language_;
    bool auto_start_ = false;
    bool close_on_blur_ = true;
    bool debug_log_ = false;
    QString launcher_hotkey_;
    QString clipboard_hotkey_;
    QString quick_paste_hotkey_;
    int max_entries_ = 200;
    int max_chars_ = 10000;
    bool monitoring_enabled_ = true;
    bool simulate_paste_on_activate_ = false;
};

} // namespace quickdeck
