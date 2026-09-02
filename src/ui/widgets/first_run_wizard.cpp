#include "ui/widgets/first_run_wizard.h"

#include <QFormLayout>
#include <QLabel>
#include <QKeySequenceEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWizardPage>

namespace quickdeck {

FirstRunWizard::FirstRunWizard(ApplicationContext &context,
                               LauncherController &launcher,
                               QWidget *parent)
    : QWizard(parent)
    , context_(context)
    , launcher_(launcher)
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
    welcome_layout->addWidget(new QLabel(tr("Let's confirm your shortcuts.")));
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
    launcher_status_ = new QLabel();
    clipboard_status_ = new QLabel();

    form->addRow(tr("Launcher"), launcher_hotkey_);
    form->addRow(QString(), launcher_status_);
    form->addRow(tr("Clipboard panel"), clipboard_hotkey_);
    form->addRow(QString(), clipboard_status_);

    auto update_status = [this]() {
        const QKeySequence launcher_seq = launcher_hotkey_->keySequence();
        const QKeySequence clipboard_seq = clipboard_hotkey_->keySequence();

        const Result<bool> launcher_ok = context_.platform().is_hotkey_available(launcher_seq);
        const Result<bool> clipboard_ok = context_.platform().is_hotkey_available(clipboard_seq);

        launcher_status_->setText(launcher_ok.is_ok() && launcher_ok.value()
                                      ? tr("Available")
                                      : tr("Conflict detected — choose another shortcut"));
        clipboard_status_->setText(clipboard_ok.is_ok() && clipboard_ok.value()
                                       ? tr("Available")
                                       : tr("Conflict detected — choose another shortcut"));
    };

    connect(launcher_hotkey_, &QKeySequenceEdit::keySequenceChanged, this, update_status);
    connect(clipboard_hotkey_, &QKeySequenceEdit::keySequenceChanged, this, update_status);
    update_status();
    addPage(hotkeys);

    auto *done = new QWizardPage();
    done->setTitle(tr("Ready"));
    auto *done_layout = new QVBoxLayout(done);
    done_layout->addWidget(new QLabel(tr("QuickDeck will stay in the system tray.")));
    addPage(done);
}

bool FirstRunWizard::validate_hotkeys()
{
    const QKeySequence launcher_seq = launcher_hotkey_->keySequence();
    const QKeySequence clipboard_seq = clipboard_hotkey_->keySequence();

    const Result<bool> launcher_ok = context_.platform().is_hotkey_available(launcher_seq);
    const Result<bool> clipboard_ok = context_.platform().is_hotkey_available(clipboard_seq);

    if ((launcher_ok.is_ok() && launcher_ok.value()) &&
        (clipboard_ok.is_ok() && clipboard_ok.value())) {
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
    context_.settings().set_string(
        QStringLiteral("launcher.hotkey"),
        launcher_hotkey_->keySequence().toString(QKeySequence::PortableText));
    context_.settings().set_string(
        QStringLiteral("clipboard.hotkey"),
        clipboard_hotkey_->keySequence().toString(QKeySequence::PortableText));
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
