#pragma once

#include "app/application_context.h"
#include "core/result.h"
#include "services/locale_service.h"

#include <QDialog>

class QCheckBox;
class QComboBox;
class QKeySequenceEdit;
class QQmlApplicationEngine;
class QPushButton;
class QSpinBox;
class QTabWidget;

namespace quickdeck {

class LauncherController;

class SettingsWindow : public QDialog {
    Q_OBJECT

public:
    SettingsWindow(ApplicationContext &context, LauncherController &launcher,
                   LocaleService &locale, QQmlApplicationEngine &qml_engine,
                   QWidget *parent = nullptr);

public slots:
    void show();
    void retranslate_ui();

protected:
    void changeEvent(QEvent *event) override;

private:
    void build_ui();
    void apply_style();
    void load_values();
    void save_values();
    [[nodiscard]] bool validate_hotkeys() const;

    ApplicationContext &context_;
    LauncherController &launcher_;
    LocaleService &locale_;
    QQmlApplicationEngine &qml_engine_;
    QTabWidget *tabs_ = nullptr;
    QCheckBox *auto_start_ = nullptr;
    QCheckBox *close_on_blur_ = nullptr;
    QComboBox *language_combo_ = nullptr;
    QKeySequenceEdit *launcher_hotkey_ = nullptr;
    QKeySequenceEdit *clipboard_hotkey_ = nullptr;
    QSpinBox *max_entries_ = nullptr;
    QSpinBox *max_chars_ = nullptr;
    QCheckBox *monitoring_enabled_ = nullptr;
    QPushButton *save_button_ = nullptr;
    QPushButton *cancel_button_ = nullptr;
    QPushButton *refresh_button_ = nullptr;
};

} // namespace quickdeck
