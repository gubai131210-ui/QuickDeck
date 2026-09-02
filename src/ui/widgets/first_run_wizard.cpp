#include "ui/widgets/first_run_wizard.h"

#include <QComboBox>
#include <QFormLayout>
#include <QLabel>
#include <QKeySequenceEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSet>
#include <QVBoxLayout>
#include <QWizardPage>

#include <algorithm>

namespace quickdeck {

FirstRunWizard::FirstRunWizard(ApplicationContext &context,
                               LauncherController &launcher,
                               LocaleService &locale,
                               QQmlApplicationEngine &qml_engine,
                               QWidget *parent)
    : QWizard(parent)
    , context_(context)
    , launcher_(launcher)
    , locale_(locale)
    , qml_engine_(qml_engine)
{
    setWindowTitle(tr("Welcome to QuickDeck"));
    setWizardStyle(QWizard::ModernStyle);
    setOption(QWizard::NoBackButtonOnStartPage, true);
    build_pages();
}

void FirstRunWizard::build_pages()
{
    auto *welcome = new QWizardPage();
    welcome->setTitle(tr("Welcome"));
    auto *welcome_layout = new QVBoxLayout(welcome);
    welcome_layout->addWidget(new QLabel(
        tr("QuickDeck helps you launch apps and manage clipboard history with global hotkeys.")));
    welcome_layout->addWidget(new QLabel(tr("Choose your language and confirm shortcuts.")));

    language_combo_ = new QComboBox();
    for (const QString &code : LocaleService::supported_languages()) {
        language_combo_->addItem(LocaleService::display_name(code), code);
    }
    const Result<QString> saved_language =
        context_.settings().get_string(QStringLiteral("general.language"),
                                       LocaleService::detect_system_language());
    if (saved_language.is_ok()) {
        const int index =
            language_combo_->findData(LocaleService::normalize_language(saved_language.value()));
        if (index >= 0) {
            language_combo_->setCurrentIndex(index);
        }
    }
    auto *language_form = new QFormLayout();
    language_form->addRow(tr("Language"), language_combo_);
    welcome_layout->addLayout(language_form);
    addPage(welcome);

    auto *hotkeys = new QWizardPage();
    hotkeys->setTitle(tr("Global Hotkeys"));
    auto *form = new QFormLayout(hotkeys);

    launcher_hotkey_ = new QKeySequenceEdit(
        QKeySequence(context_.settings()
                         .get_string(QStringLiteral("launcher.hotkey"), QStringLiteral("Alt+Space"))
                         .value()));
    clipboard_hotkey_ = new QKeySequenceEdit(
        QKeySequence(context_.settings()
                         .get_string(QStringLiteral("clipboard.hotkey"),
                                      QStringLiteral("Ctrl+Shift+V"))
                         .value()));
    quick_paste_hotkey_ = new QKeySequenceEdit(
        QKeySequence(context_.settings()
                         .get_string(QStringLiteral("clipboard.quick_paste_hotkey"),
                                      QStringLiteral("Ctrl+Alt+V"))
                         .value()));
    launcher_status_ = new QLabel();
    clipboard_status_ = new QLabel();
    quick_paste_status_ = new QLabel();

    form->addRow(tr("Launcher"), launcher_hotkey_);
    form->addRow(QString(), launcher_status_);
    form->addRow(tr("Clipboard panel"), clipboard_hotkey_);
    form->addRow(QString(), clipboard_status_);
    form->addRow(tr("Quick paste"), quick_paste_hotkey_);
    form->addRow(QString(), quick_paste_status_);

    auto update_status = [this]() {
        const QKeySequence launcher_seq = launcher_hotkey_->keySequence();
        const QKeySequence clipboard_seq = clipboard_hotkey_->keySequence();
        const QKeySequence quick_paste_seq = quick_paste_hotkey_->keySequence();

        const Result<bool> launcher_ok = context_.platform().is_hotkey_available(launcher_seq);
        const Result<bool> clipboard_ok = context_.platform().is_hotkey_available(clipboard_seq);

        launcher_status_->setText(launcher_ok.is_ok() && launcher_ok.value()
                                      ? tr("Available")
                                      : tr("Conflict detected — choose another shortcut"));
        clipboard_status_->setText(clipboard_ok.is_ok() && clipboard_ok.value()
                                       ? tr("Available")
                                       : tr("Conflict detected — choose another shortcut"));

        if (quick_paste_seq.isEmpty()) {
            quick_paste_status_->setText(tr("Disabled (empty shortcut)"));
        } else {
            const Result<bool> quick_paste_ok =
                context_.platform().is_hotkey_available(quick_paste_seq);
            quick_paste_status_->setText(quick_paste_ok.is_ok() && quick_paste_ok.value()
                                             ? tr("Available")
                                             : tr("Conflict detected — choose another shortcut"));
        }

        const QStringList sequences = {
            launcher_seq.toString(QKeySequence::PortableText),
            clipboard_seq.toString(QKeySequence::PortableText),
            quick_paste_seq.toString(QKeySequence::PortableText),
        };
        QSet<QString> unique;
        for (const QString &sequence : sequences) {
            if (!sequence.isEmpty()) {
                unique.insert(sequence);
            }
        }
        const int non_empty_count =
            static_cast<int>(std::count_if(sequences.begin(), sequences.end(),
                                             [](const QString &s) { return !s.isEmpty(); }));
        if (unique.size() < non_empty_count) {
            launcher_status_->setText(tr("Duplicate shortcut — each hotkey must be unique"));
        }
    };

    connect(launcher_hotkey_, &QKeySequenceEdit::keySequenceChanged, this, update_status);
    connect(clipboard_hotkey_, &QKeySequenceEdit::keySequenceChanged, this, update_status);
    connect(quick_paste_hotkey_, &QKeySequenceEdit::keySequenceChanged, this, update_status);
    connect(this, &QWizard::currentIdChanged, this, [this](int id) {
        if (id == 2) {
            update_ready_summary();
        }
    });
    update_status();
    addPage(hotkeys);

    auto *done = new QWizardPage();
    done->setTitle(tr("Ready"));
    auto *done_layout = new QVBoxLayout(done);
    done_layout->addWidget(new QLabel(tr("QuickDeck will stay in the system tray.")));
    ready_summary_ = new QLabel();
    ready_summary_->setWordWrap(true);
    done_layout->addWidget(ready_summary_);
    addPage(done);
}

void FirstRunWizard::update_ready_summary()
{
    const QString launcher = launcher_hotkey_->keySequence().toString(QKeySequence::NativeText);
    const QString clipboard = clipboard_hotkey_->keySequence().toString(QKeySequence::NativeText);
    const QString quick_paste = quick_paste_hotkey_->keySequence().toString(QKeySequence::NativeText);

    QString summary = tr("Launcher: %1\nClipboard panel: %2").arg(launcher, clipboard);
    if (!quick_paste.isEmpty()) {
        summary += tr("\nQuick paste: %1").arg(quick_paste);
    }
    ready_summary_->setText(summary);
}

bool FirstRunWizard::validate_hotkeys()
{
    const QKeySequence launcher_seq = launcher_hotkey_->keySequence();
    const QKeySequence clipboard_seq = clipboard_hotkey_->keySequence();
    const QKeySequence quick_paste_seq = quick_paste_hotkey_->keySequence();

    const QStringList sequences = {
        launcher_seq.toString(QKeySequence::PortableText),
        clipboard_seq.toString(QKeySequence::PortableText),
        quick_paste_seq.toString(QKeySequence::PortableText),
    };
    QSet<QString> unique;
    for (const QString &sequence : sequences) {
        if (!sequence.isEmpty()) {
            unique.insert(sequence);
        }
    }
    const int non_empty_count =
        static_cast<int>(std::count_if(sequences.begin(), sequences.end(),
                                       [](const QString &s) { return !s.isEmpty(); }));
    if (unique.size() < non_empty_count) {
        const QMessageBox::StandardButton answer = QMessageBox::warning(
            this, tr("Duplicate Hotkeys"),
            tr("Each shortcut must be unique. Continue anyway?"),
            QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if (answer != QMessageBox::Yes) {
            return false;
        }
    }

    const Result<bool> launcher_ok = context_.platform().is_hotkey_available(launcher_seq);
    const Result<bool> clipboard_ok = context_.platform().is_hotkey_available(clipboard_seq);
    const Result<bool> quick_paste_ok = quick_paste_seq.isEmpty()
                                            ? Result<bool>::ok(true)
                                            : context_.platform().is_hotkey_available(quick_paste_seq);

    if ((launcher_ok.is_ok() && launcher_ok.value()) &&
        (clipboard_ok.is_ok() && clipboard_ok.value()) &&
        (quick_paste_ok.is_ok() && quick_paste_ok.value())) {
        return true;
    }

    const QMessageBox::StandardButton answer = QMessageBox::warning(
        this, tr("Hotkey Conflict"),
        tr("One or more shortcuts are unavailable on this system. Continue anyway?"),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    return answer == QMessageBox::Yes;
}

void FirstRunWizard::persist_settings()
{
    const QString language_code = language_combo_->currentData().toString();
    locale_.set_language(context_.settings(), language_code, &qml_engine_);

    context_.settings().set_string(
        QStringLiteral("launcher.hotkey"),
        launcher_hotkey_->keySequence().toString(QKeySequence::PortableText));
    context_.settings().set_string(
        QStringLiteral("clipboard.hotkey"),
        clipboard_hotkey_->keySequence().toString(QKeySequence::PortableText));
    context_.settings().set_string(
        QStringLiteral("clipboard.quick_paste_hotkey"),
        quick_paste_hotkey_->keySequence().toString(QKeySequence::PortableText));
    context_.settings().set_bool(QStringLiteral("setup.completed"), true);
    launcher_.reload_settings();
}

bool FirstRunWizard::run()
{
    while (true) {
        if (exec() != QDialog::Accepted) {
            launcher_.reload_settings();
            return false;
        }

        if (validate_hotkeys()) {
            persist_settings();
            return true;
        }

        setCurrentId(1);
    }
}

} // namespace quickdeck
