#pragma once

#include "app/application_context.h"

#include <QDialog>

class QCheckBox;
class QKeySequenceEdit;
class QSpinBox;
class QTabWidget;

namespace quickdeck {

class LauncherController;

class SettingsWindow : public QDialog {
    Q_OBJECT

public:
    explicit SettingsWindow(ApplicationContext &context, LauncherController &launcher,
                            QWidget *parent = nullptr);

public slots:
    void show();

private:
    void build_ui();
    void load_values();
    void save_values();
    [[nodiscard]] bool validate_hotkeys() const;

    ApplicationContext &context_;
    LauncherController &launcher_;
    QTabWidget *tabs_ = nullptr;
    QCheckBox *auto_start_ = nullptr;
    QCheckBox *close_on_blur_ = nullptr;
    QKeySequenceEdit *launcher_hotkey_ = nullptr;
    QKeySequenceEdit *clipboard_hotkey_ = nullptr;
    QSpinBox *max_entries_ = nullptr;
    QSpinBox *max_chars_ = nullptr;
    QCheckBox *monitoring_enabled_ = nullptr;
};

} // namespace quickdeck
