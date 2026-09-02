#include "ui/widgets/settings_window.h"

#include <QCheckBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QKeySequenceEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QTabWidget>
#include <QVBoxLayout>

namespace quickdeck {

SettingsWindow::SettingsWindow(ApplicationContext &context, QWidget *parent)
    : QDialog(parent)
    , context_(context)
{
    setWindowTitle(tr("QuickDeck Settings"));
    resize(480, 360);
    build_ui();
    load_values();
}

void SettingsWindow::build_ui()
{
    tabs_ = new QTabWidget(this);

    auto *general_tab = new QWidget();
    auto *general_layout = new QFormLayout(general_tab);
    auto *auto_start = new QCheckBox(tr("Start at login"));
    auto *close_on_blur = new QCheckBox(tr("Close overlay when focus is lost"));
    auto *refresh_button = new QPushButton(tr("Refresh application index"));
    general_layout->addRow(auto_start);
    general_layout->addRow(close_on_blur);
    general_layout->addRow(refresh_button);
    tabs_->addTab(general_tab, tr("General"));

    auto *hotkey_tab = new QWidget();
    auto *hotkey_layout = new QFormLayout(hotkey_tab);
    auto *launcher_hotkey = new QKeySequenceEdit();
    auto *clipboard_hotkey = new QKeySequenceEdit();
    hotkey_layout->addRow(tr("Launcher hotkey"), launcher_hotkey);
    hotkey_layout->addRow(tr("Clipboard hotkey"), clipboard_hotkey);
    tabs_->addTab(hotkey_tab, tr("Hotkeys"));

    auto *clipboard_tab = new QWidget();
    auto *clipboard_layout = new QFormLayout(clipboard_tab);
    auto *max_entries = new QSpinBox();
    max_entries->setRange(10, 10000);
    auto *max_chars = new QSpinBox();
    max_chars->setRange(100, 1000000);
    auto *monitoring_enabled = new QCheckBox(tr("Enable clipboard monitoring"));
    clipboard_layout->addRow(tr("Max entries"), max_entries);
    clipboard_layout->addRow(tr("Max characters"), max_chars);
    clipboard_layout->addRow(monitoring_enabled);
    tabs_->addTab(clipboard_tab, tr("Clipboard"));

    auto *root = new QVBoxLayout(this);
    root->addWidget(tabs_);

    auto *buttons = new QHBoxLayout();
    auto *save_button = new QPushButton(tr("Save"));
    auto *cancel_button = new QPushButton(tr("Cancel"));
    buttons->addStretch();
    buttons->addWidget(save_button);
    buttons->addWidget(cancel_button);
    root->addLayout(buttons);

    connect(save_button, &QPushButton::clicked, this, [this]() {
        save_values();
        accept();
    });
    connect(cancel_button, &QPushButton::clicked, this, &QDialog::reject);
    connect(refresh_button, &QPushButton::clicked, this, [this]() {
        context_.app_indexer().refresh_catalog();
    });

    general_tab->setProperty("auto_start", QVariant::fromValue(auto_start));
    general_tab->setProperty("close_on_blur", QVariant::fromValue(close_on_blur));
    hotkey_tab->setProperty("launcher_hotkey", QVariant::fromValue(launcher_hotkey));
    hotkey_tab->setProperty("clipboard_hotkey", QVariant::fromValue(clipboard_hotkey));
    clipboard_tab->setProperty("max_entries", QVariant::fromValue(max_entries));
    clipboard_tab->setProperty("max_chars", QVariant::fromValue(max_chars));
    clipboard_tab->setProperty("monitoring_enabled", QVariant::fromValue(monitoring_enabled));
}

void SettingsWindow::load_values()
{
    ISettingsStore &store = context_.settings();
    const QWidget *general_tab = tabs_->widget(0);
    const QWidget *hotkey_tab = tabs_->widget(1);
    const QWidget *clipboard_tab = tabs_->widget(2);

    general_tab->findChild<QCheckBox *>()->setChecked(
        store.get_bool(QStringLiteral("general.auto_start"), false).value());
    general_tab->findChildren<QCheckBox *>().at(1)->setChecked(
        store.get_bool(QStringLiteral("general.close_on_blur"), true).value());

    hotkey_tab->findChild<QKeySequenceEdit *>()->setKeySequence(QKeySequence(
        store.get_string(QStringLiteral("launcher.hotkey"), QStringLiteral("Alt+Space")).value()));
    hotkey_tab->findChildren<QKeySequenceEdit *>().at(1)->setKeySequence(QKeySequence(
        store.get_string(QStringLiteral("clipboard.hotkey"), QStringLiteral("Ctrl+Shift+V"))
            .value()));

    clipboard_tab->findChild<QSpinBox *>()->setValue(
        store.get_int(QStringLiteral("clipboard.max_entries"), 200).value());
    clipboard_tab->findChildren<QSpinBox *>().at(1)->setValue(
        store.get_int(QStringLiteral("clipboard.max_char_length"), 10000).value());
    clipboard_tab->findChild<QCheckBox *>()->setChecked(
        store.get_bool(QStringLiteral("clipboard.monitoring_enabled"), true).value());
}

void SettingsWindow::save_values()
{
    ISettingsStore &store = context_.settings();
    const QWidget *general_tab = tabs_->widget(0);
    const QWidget *hotkey_tab = tabs_->widget(1);
    const QWidget *clipboard_tab = tabs_->widget(2);

    store.set_bool(QStringLiteral("general.auto_start"),
                   general_tab->findChild<QCheckBox *>()->isChecked());
    store.set_bool(QStringLiteral("general.close_on_blur"),
                   general_tab->findChildren<QCheckBox *>().at(1)->isChecked());

    store.set_string(QStringLiteral("launcher.hotkey"),
                     hotkey_tab->findChild<QKeySequenceEdit *>()->keySequence().toString());
    store.set_string(QStringLiteral("clipboard.hotkey"),
                     hotkey_tab->findChildren<QKeySequenceEdit *>().at(1)->keySequence().toString());

    store.set_int(QStringLiteral("clipboard.max_entries"),
                  clipboard_tab->findChild<QSpinBox *>()->value());
    store.set_int(QStringLiteral("clipboard.max_char_length"),
                  clipboard_tab->findChildren<QSpinBox *>().at(1)->value());
    store.set_bool(QStringLiteral("clipboard.monitoring_enabled"),
                   clipboard_tab->findChild<QCheckBox *>()->isChecked());

    context_.platform().set_auto_start_enabled(
        store.get_bool(QStringLiteral("general.auto_start"), false).value());
}

void SettingsWindow::show()
{
    load_values();
    QDialog::show();
}

} // namespace quickdeck
