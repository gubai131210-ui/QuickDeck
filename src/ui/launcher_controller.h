#pragma once

#include "app/application_context.h"
#include "core/domain/types.h"
#include "ui/models/app_search_model.h"
#include "ui/models/clipboard_history_model.h"
#include "ui/models/command_search_model.h"

#include <QQmlApplicationEngine>

namespace quickdeck {

class SettingsController;
class LocaleService;

class LauncherController : public QObject {
    Q_OBJECT
    Q_PROPERTY(int modeValue READ mode_value NOTIFY modeChanged)
    Q_PROPERTY(bool visible READ visible NOTIFY visibleChanged)
    Q_PROPERTY(bool commandMode READ command_mode NOTIFY commandModeChanged)
    Q_PROPERTY(QString query READ query WRITE setQuery NOTIFY queryChanged)
    Q_PROPERTY(int selectedIndex READ selected_index WRITE set_selected_index NOTIFY selectedIndexChanged)
    Q_PROPERTY(int itemCount READ item_count NOTIFY itemCountChanged)
    Q_PROPERTY(bool closeOnBlur READ close_on_blur NOTIFY closeOnBlurChanged)
    Q_PROPERTY(AppSearchModel *appModel READ app_model CONSTANT)
    Q_PROPERTY(ClipboardHistoryModel *clipboardModel READ clipboard_model CONSTANT)
    Q_PROPERTY(CommandSearchModel *commandModel READ command_model CONSTANT)

public:
    explicit LauncherController(ApplicationContext &context, QObject *parent = nullptr);

    void set_settings_controller(SettingsController *settings_controller);
    void set_locale_service(LocaleService *locale_service);
    void set_qml_engine(QQmlApplicationEngine *qml_engine);

    [[nodiscard]] int mode_value() const { return static_cast<int>(mode_); }
    [[nodiscard]] bool visible() const { return visible_; }
    [[nodiscard]] bool command_mode() const { return command_mode_; }
    [[nodiscard]] QString query() const { return query_; }
    [[nodiscard]] int selected_index() const { return selected_index_; }
    [[nodiscard]] int item_count() const { return item_count_; }
    [[nodiscard]] bool close_on_blur() const { return close_on_blur_; }
    [[nodiscard]] AppSearchModel *app_model() { return &app_model_; }
    [[nodiscard]] ClipboardHistoryModel *clipboard_model() { return &clipboard_model_; }
    [[nodiscard]] CommandSearchModel *command_model() { return &command_model_; }

    Q_INVOKABLE void show_search();
    Q_INVOKABLE void show_clipboard();
    Q_INVOKABLE void switch_mode(int mode_value);
    Q_INVOKABLE void hide();
    Q_INVOKABLE void dismiss();
    Q_INVOKABLE void confirm_hide();
    Q_INVOKABLE void activate_selected(int index, bool simulate_paste = false);
    Q_INVOKABLE void move_selection(int delta);
    Q_INVOKABLE void toggle_pin_at(int index);
    Q_INVOKABLE void delete_selected_at(int index);
    Q_INVOKABLE void quick_paste_latest();
    Q_INVOKABLE void reload_settings();
    Q_INVOKABLE void register_hotkeys();

    void setQuery(const QString &query);
    void set_selected_index(int index);

signals:
    void modeChanged();
    void visibleChanged();
    void commandModeChanged();
    void queryChanged();
    void selectedIndexChanged();
    void itemCountChanged();
    void closeOnBlurChanged();
    void hideRequested();
    void launchFailed(const QString &app_name, const QString &error_message);
    void quickPasteFailed(const QString &error_code);
    void openSettingsRequested();
    void hotkeyRegistrationFailed(const QString &hotkey_name, const QString &detail);

private:
    void refresh_results();
    void setup_hotkeys();
    void update_item_count();
    void load_runtime_settings();
    void set_command_mode(bool enabled);
    void execute_command(const CommandItem &command);

    ApplicationContext &context_;
    SettingsController *settings_controller_ = nullptr;
    LocaleService *locale_service_ = nullptr;
    QQmlApplicationEngine *qml_engine_ = nullptr;
    AppSearchModel app_model_;
    ClipboardHistoryModel clipboard_model_;
    CommandSearchModel command_model_;
    QList<AppEntry> app_results_;
    QList<ClipboardEntry> clipboard_results_;
    QVector<CommandItem> command_results_;
    LauncherMode mode_ = LauncherMode::Search;
    bool visible_ = false;
    bool command_mode_ = false;
    bool close_on_blur_ = true;
    int selected_index_ = 0;
    int item_count_ = 0;
    QString query_;
};

} // namespace quickdeck
