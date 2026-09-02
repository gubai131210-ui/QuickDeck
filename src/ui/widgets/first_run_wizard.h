#pragma once

#include "app/application_context.h"
#include "services/locale_service.h"
#include "ui/launcher_controller.h"

#include <QWizard>

class QComboBox;
class QKeySequenceEdit;
class QLabel;

class QQmlApplicationEngine;

namespace quickdeck {

class FirstRunWizard : public QWizard {
    Q_OBJECT

public:
    explicit FirstRunWizard(ApplicationContext &context, LauncherController &launcher,
                            LocaleService &locale, QQmlApplicationEngine &qml_engine,
                            QWidget *parent = nullptr);

    [[nodiscard]] bool run();

private:
    void build_pages();
    [[nodiscard]] bool validate_hotkeys();
    void persist_settings();
    void update_ready_summary();

    ApplicationContext &context_;
    LauncherController &launcher_;
    LocaleService &locale_;
    QQmlApplicationEngine &qml_engine_;
    QComboBox *language_combo_ = nullptr;
    QKeySequenceEdit *launcher_hotkey_ = nullptr;
    QKeySequenceEdit *clipboard_hotkey_ = nullptr;
    QKeySequenceEdit *quick_paste_hotkey_ = nullptr;
    QLabel *launcher_status_ = nullptr;
    QLabel *clipboard_status_ = nullptr;
    QLabel *quick_paste_status_ = nullptr;
    QLabel *ready_summary_ = nullptr;
};

} // namespace quickdeck
