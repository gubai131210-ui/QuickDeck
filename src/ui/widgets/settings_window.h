#pragma once

#include "app/application_context.h"

#include <QDialog>

class QTabWidget;

namespace quickdeck {

class SettingsWindow : public QDialog {
    Q_OBJECT

public:
    explicit SettingsWindow(ApplicationContext &context, QWidget *parent = nullptr);

public slots:
    void show();

private:
    void build_ui();
    void load_values();
    void save_values();

    ApplicationContext &context_;
    QTabWidget *tabs_ = nullptr;
};

} // namespace quickdeck
