#include "ui/widgets/settings_window.h"

#include "ui/launcher_controller.h"

#include <QCheckBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QKeySequenceEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QTabWidget>
#include <QVBoxLayout>

namespace quickdeck {

SettingsWindow::SettingsWindow(ApplicationContext &context,
                             LauncherController &launcher,
                             QWidget *parent)
    : QDialog(parent)
    , context_(context)
    , launcher_(launcher)
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
    auto_start_ = new QCheckBox(tr("Start at login"));
    close_on_blur_ = new QCheckBox(tr("Close overlay when focus is lost"));
    auto *refresh_button = new QPushButton(tr("Refresh application index"));
    general_layout->addRow(auto_start_);
    general_layout->addRow(close_on_blur_);
    general_layout->addRow(refresh_button);
    tabs_->addTab(general_tab, tr("General"));

    auto *hotkey_tab = new QWidget();
    auto *hotkey_layout = new QFormLayout(hotkey_tab);
    launcher_hotkey_ = new QKeySequenceEdit();
    clipboard_hotkey_ = new QKeySequenceEdit();
    hotkey_layout->addRow(tr("Launcher hotkey"), launcher_hotkey_);
    hotkey_layout->addRow(tr("Clipboard hotkey"), clipboard_hotkey_);
    tabs_->addTab(hotkey_tab, tr("Hotkeys"));

    auto *clipboard_tab = new QWidget();
    auto *clipboard_layout = new QFormLayout(clipboard_tab);
    max_entries_ = new QSpinBox();
    max_entries_->setRange(10, 10000);
    max_chars_ = new QSpinBox();
    max_chars_->setRange(100, 1000000);
    monitoring_enabled_ = new QCheckBox(tr("Enable clipboard monitoring"));
    clipboard_layout->addRow(tr("Max entries"), max_entries_);
    clipboard_layout->addRow(tr("Max characters"), max_chars_);
    clipboard_layout->addRow(monitoring_enabled_);
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
        if (!validate_hotkeys()) {
            return;
        }
        save_values();
        launcher_.reload_settings();
        accept();
    });
    connect(cancel_button, &QPushButton::clicked, this, &QDialog::reject);
    connect(refresh_button, &QPushButton::clicked, this, [this]() {
        const Result<int> result = context_.app_indexer().refresh_catalog();
        if (result.is_err()) {
            QMessageBox::warning(this, tr("Index Refresh"), result.error());
        }
    });
}

bool SettingsWindow::validate_hotkeys() const
{
    ISettingsStore &store = context_.settings();
    const QString stored_launcher =
        store.get_string(QStringLiteral("launcher.hotkey"), QStringLiteral("Alt+Space")).value();
    const QString stored_clipboard =
        store.get_string(QStringLiteral("clipboard.hotkey"), QStringLiteral("Ctrl+Shift+V"))
            .value();
    const QString new_launcher =
        launcher_hotkey_->keySequence().toString(QKeySequence::PortableText);
    const QString new_clipboard =
        clipboard_hotkey_->keySequence().toString(QKeySequence::PortableText);

    auto needs_probe = [&](const QString &stored, const QString &updated) {
        return stored != updated;
    };

    if (needs_probe(stored_launcher, new_launcher)) {
        const Result<bool> launcher_ok =
            context_.platform().is_hotkey_available(launcher_hotkey_->keySequence());
        if (launcher_ok.is_err() || !launcher_ok.value()) {
            const QMessageBox::StandardButton answer = QMessageBox::warning(
                const_cast<SettingsWindow *>(this), tr("Hotkey Conflict"),
                tr("Launcher shortcut appears unavailable. Save anyway?"),
                QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
            if (answer != QMessageBox::Yes) {
                return false;
            }
        }
    }

    if (needs_probe(stored_clipboard, new_clipboard)) {
        const Result<bool> clipboard_ok =
            context_.platform().is_hotkey_available(clipboard_hotkey_->keySequence());
        if (clipboard_ok.is_err() || !clipboard_ok.value()) {
            const QMessageBox::StandardButton answer = QMessageBox::warning(
                const_cast<SettingsWindow *>(this), tr("Hotkey Conflict"),
                tr("Clipboard shortcut appears unavailable. Save anyway?"),
                QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
            if (answer != QMessageBox::Yes) {
                return false;
            }
        }
    }

    if (new_launcher == new_clipboard) {
        const QMessageBox::StandardButton answer = QMessageBox::warning(
            const_cast<SettingsWindow *>(this), tr("Duplicate Hotkeys"),
            tr("Launcher and clipboard shortcuts cannot be identical. Save anyway?"),
            QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        return answer == QMessageBox::Yes;
    }

    return true;
}

void SettingsWindow::load_values()
{
    ISettingsStore &store = context_.settings();
    auto_start_->setChecked(store.get_bool(QStringLiteral("general.auto_start"), false).value());
    close_on_blur_->setChecked(
        store.get_bool(QStringLiteral("general.close_on_blur"), true).value());

    launcher_hotkey_->setKeySequence(QKeySequence(
        store.get_string(QStringLiteral("launcher.hotkey"), QStringLiteral("Alt+Space")).value()));
    clipboard_hotkey_->setKeySequence(QKeySequence(
        store.get_string(QStringLiteral("clipboard.hotkey"), QStringLiteral("Ctrl+Shift+V"))
            .value()));

    max_entries_->setValue(store.get_int(QStringLiteral("clipboard.max_entries"), 200).value());
    max_chars_->setValue(store.get_int(QStringLiteral("clipboard.max_char_length"), 10000).value());
    monitoring_enabled_->setChecked(
        store.get_bool(QStringLiteral("clipboard.monitoring_enabled"), true).value());
}

void SettingsWindow::save_values()
{
    ISettingsStore &store = context_.settings();
    store.set_bool(QStringLiteral("general.auto_start"), auto_start_->isChecked());
    store.set_bool(QStringLiteral("general.close_on_blur"), close_on_blur_->isChecked());

    store.set_string(QStringLiteral("launcher.hotkey"),
                     launcher_hotkey_->keySequence().toString(QKeySequence::PortableText));
    store.set_string(QStringLiteral("clipboard.hotkey"),
                     clipboard_hotkey_->keySequence().toString(QKeySequence::PortableText));

    store.set_int(QStringLiteral("clipboard.max_entries"), max_entries_->value());
    store.set_int(QStringLiteral("clipboard.max_char_length"), max_chars_->value());
    store.set_bool(QStringLiteral("clipboard.monitoring_enabled"), monitoring_enabled_->isChecked());

    context_.platform().set_auto_start_enabled(
        store.get_bool(QStringLiteral("general.auto_start"), false).value());
}

void SettingsWindow::show()
{
    load_values();
    QDialog::show();
}

} // namespace quickdeck
