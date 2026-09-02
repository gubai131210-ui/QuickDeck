#pragma once

#include "app/application_context.h"
#include "ui/launcher_controller.h"

#include <QWizard>

class QKeySequenceEdit;
class QLabel;

namespace quickdeck {

class FirstRunWizard : public QWizard {
    Q_OBJECT

public:
    explicit FirstRunWizard(ApplicationContext &context, LauncherController &launcher,
                            QWidget *parent = nullptr);

    [[nodiscard]] bool run();

private:
    void build_pages();
    [[nodiscard]] bool validate_hotkeys();
    void persist_settings();

    ApplicationContext &context_;
    LauncherController &launcher_;
    QKeySequenceEdit *launcher_hotkey_ = nullptr;
    QKeySequenceEdit *clipboard_hotkey_ = nullptr;
    QLabel *launcher_status_ = nullptr;
    QLabel *clipboard_status_ = nullptr;
};

} // namespace quickdeck
