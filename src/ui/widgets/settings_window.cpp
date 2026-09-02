#include "ui/widgets/settings_window.h"

#include "ui/launcher_controller.h"

#include <QCheckBox>
#include <QComboBox>
#include <QEvent>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QKeySequenceEdit>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QTabWidget>
#include <QVBoxLayout>

namespace quickdeck {

SettingsWindow::SettingsWindow(ApplicationContext &context,
                             LauncherController &launcher,
                             LocaleService &locale,
                             QQmlApplicationEngine &qml_engine,
                             QWidget *parent)
    : QDialog(parent)
    , context_(context)
    , launcher_(launcher)
    , locale_(locale)
    , qml_engine_(qml_engine)
{
    resize(520, 400);
    build_ui();
    apply_style();
    retranslate_ui();
    load_values();
}

void SettingsWindow::apply_style()
{
    setStyleSheet(QStringLiteral(
        "QDialog { background: #F8FAFC; color: #0F172A; }"
        "QTabWidget::pane { border: 1px solid #E2E8F0; border-radius: 12px; background: #FFFFFF; top: -1px; }"
        "QTabBar::tab { background: #E2E8F0; color: #64748B; border-radius: 10px; padding: 8px 16px; margin: 4px; }"
        "QTabBar::tab:selected { background: #E0F2FE; color: #0284C7; }"
        "QLineEdit, QSpinBox, QKeySequenceEdit, QComboBox {"
        "  background: #F1F5F9; border: 1px solid #E2E8F0; border-radius: 10px; padding: 6px 10px; }"
        "QLineEdit:focus, QSpinBox:focus, QKeySequenceEdit:focus, QComboBox:focus { border: 2px solid #38BDF8; }"
        "QPushButton { background: #38BDF8; color: white; border: none; border-radius: 10px; padding: 8px 18px; }"
        "QPushButton:hover { background: #0EA5E9; }"
        "QPushButton#cancelButton { background: #E2E8F0; color: #334155; }"
        "QCheckBox { spacing: 8px; }"));
}

void SettingsWindow::build_ui()
{
    tabs_ = new QTabWidget(this);

    auto *general_tab = new QWidget();
    auto *general_layout = new QFormLayout(general_tab);
    auto_start_ = new QCheckBox();
    close_on_blur_ = new QCheckBox();
    language_combo_ = new QComboBox();
    refresh_button_ = new QPushButton();
    for (const QString &code : LocaleService::supported_languages()) {
        language_combo_->addItem(LocaleService::display_name(code), code);
    }
    general_layout->addRow(tr("Language"), language_combo_);
    general_layout->addRow(auto_start_);
    general_layout->addRow(close_on_blur_);
    general_layout->addRow(refresh_button_);
    tabs_->addTab(general_tab, QString());

    auto *hotkey_tab = new QWidget();
    auto *hotkey_layout = new QFormLayout(hotkey_tab);
    launcher_hotkey_ = new QKeySequenceEdit();
    clipboard_hotkey_ = new QKeySequenceEdit();
    hotkey_layout->addRow(tr("Launcher hotkey"), launcher_hotkey_);
    hotkey_layout->addRow(tr("Clipboard hotkey"), clipboard_hotkey_);
    tabs_->addTab(hotkey_tab, QString());

    auto *clipboard_tab = new QWidget();
    auto *clipboard_layout = new QFormLayout(clipboard_tab);
    max_entries_ = new QSpinBox();
    max_entries_->setRange(10, 10000);
    max_chars_ = new QSpinBox();
    max_chars_->setRange(100, 1000000);
    monitoring_enabled_ = new QCheckBox();
    clipboard_layout->addRow(tr("Max entries"), max_entries_);
    clipboard_layout->addRow(tr("Max characters"), max_chars_);
    clipboard_layout->addRow(monitoring_enabled_);
    tabs_->addTab(clipboard_tab, QString());

    auto *root = new QVBoxLayout(this);
    root->addWidget(tabs_);

    auto *buttons = new QHBoxLayout();
    save_button_ = new QPushButton();
    cancel_button_ = new QPushButton();
    cancel_button_->setObjectName(QStringLiteral("cancelButton"));
    buttons->addStretch();
    buttons->addWidget(save_button_);
    buttons->addWidget(cancel_button_);
    root->addLayout(buttons);

    connect(save_button_, &QPushButton::clicked, this, [this]() {
        if (!validate_hotkeys()) {
            return;
        }
        const QString previous_language = locale_.current_language();
        save_values();
        const QString selected_language =
            language_combo_->currentData().toString();
        if (selected_language != previous_language) {
            locale_.set_language(context_.settings(), selected_language, &qml_engine_);
        }
        launcher_.reload_settings();
        retranslate_ui();
        accept();
    });
    connect(cancel_button_, &QPushButton::clicked, this, &QDialog::reject);
    connect(refresh_button_, &QPushButton::clicked, this, [this]() {
        const Result<int> result = context_.app_indexer().refresh_catalog();
        if (result.is_err()) {
            QMessageBox::warning(this, tr("Index Refresh"), result.error());
        }
    });
}

void SettingsWindow::retranslate_ui()
{
    setWindowTitle(tr("QuickDeck Settings"));
    tabs_->setTabText(0, tr("General"));
    tabs_->setTabText(1, tr("Hotkeys"));
    tabs_->setTabText(2, tr("Clipboard"));

    if (auto *general_layout = qobject_cast<QFormLayout *>(tabs_->widget(0)->layout())) {
        if (QLabel *label = qobject_cast<QLabel *>(general_layout->labelForField(language_combo_))) {
            label->setText(tr("Language"));
        }
    }
    auto_start_->setText(tr("Start at login"));
    close_on_blur_->setText(tr("Close overlay when focus is lost"));
    refresh_button_->setText(tr("Refresh application index"));

    if (auto *hotkey_layout = qobject_cast<QFormLayout *>(tabs_->widget(1)->layout())) {
        if (QLabel *label = qobject_cast<QLabel *>(hotkey_layout->labelForField(launcher_hotkey_))) {
            label->setText(tr("Launcher hotkey"));
        }
        if (QLabel *label = qobject_cast<QLabel *>(hotkey_layout->labelForField(clipboard_hotkey_))) {
            label->setText(tr("Clipboard hotkey"));
        }
    }

    if (auto *clipboard_layout = qobject_cast<QFormLayout *>(tabs_->widget(2)->layout())) {
        if (QLabel *label = qobject_cast<QLabel *>(clipboard_layout->labelForField(max_entries_))) {
            label->setText(tr("Max entries"));
        }
        if (QLabel *label = qobject_cast<QLabel *>(clipboard_layout->labelForField(max_chars_))) {
            label->setText(tr("Max characters"));
        }
    }
    monitoring_enabled_->setText(tr("Enable clipboard monitoring"));

    save_button_->setText(tr("Save"));
    cancel_button_->setText(tr("Cancel"));
}

void SettingsWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        retranslate_ui();
    }
    QDialog::changeEvent(event);
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

    const Result<QString> language =
        store.get_string(QStringLiteral("general.language"), LocaleService::detect_system_language());
    const int language_index =
        language_combo_->findData(LocaleService::normalize_language(language.value()));
    if (language_index >= 0) {
        language_combo_->setCurrentIndex(language_index);
    }

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
    store.set_string(QStringLiteral("general.language"),
                     language_combo_->currentData().toString());

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
