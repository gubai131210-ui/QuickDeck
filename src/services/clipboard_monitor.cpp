#include "services/clipboard_monitor.h"

#include "services/logger.h"

#include <QApplication>
#include <QClipboard>
#include <QMimeData>

namespace quickdeck {

ClipboardMonitor::ClipboardMonitor(IClipboardRepository &clipboard,
                                 ISettingsStore &settings,
                                 QObject *parent)
    : QObject(parent)
    , clipboard_(clipboard)
    , settings_(settings)
{
}

Result<void> ClipboardMonitor::start()
{
    if (running_) {
        return Result<void>::ok();
    }

    connect(QApplication::clipboard(), &QClipboard::dataChanged, this,
            &ClipboardMonitor::on_clipboard_changed);
    running_ = true;
    QD_LOG_INFO(QStringLiteral("Clipboard monitoring started"));
    return Result<void>::ok();
}

void ClipboardMonitor::stop()
{
    if (!running_) {
        return;
    }
    disconnect(QApplication::clipboard(), &QClipboard::dataChanged, this,
               &ClipboardMonitor::on_clipboard_changed);
    running_ = false;
}

void ClipboardMonitor::set_suppress_next_change(bool suppress)
{
    suppress_next_change_ = suppress;
}

void ClipboardMonitor::on_clipboard_changed()
{
    if (suppress_next_change_) {
        suppress_next_change_ = false;
        return;
    }

    const Result<bool> enabled =
        settings_.get_bool(QStringLiteral("clipboard.monitoring_enabled"), true);
    if (enabled.is_err() || !enabled.value()) {
        return;
    }

    const QMimeData *mime = QApplication::clipboard()->mimeData();
    if (mime == nullptr || !mime->hasText()) {
        return;
    }

    QString text = mime->text();
    const Result<int> max_length =
        settings_.get_int(QStringLiteral("clipboard.max_char_length"), 10000);
    if (max_length.is_ok() && text.length() > max_length.value()) {
        text = text.left(max_length.value());
    }

    if (text.trimmed().isEmpty()) {
        return;
    }

    const Result<bool> duplicate = clipboard_.is_duplicate(text);
    if (duplicate.is_ok() && duplicate.value()) {
        return;
    }

    ClipboardEntry entry;
    entry.content = text;
    entry.content_type = QStringLiteral("text/plain");

    const Result<qint64> insert_result = clipboard_.insert(entry);
    if (insert_result.is_err()) {
        QD_LOG_ERROR(insert_result.error());
        return;
    }

    const Result<int> max_entries =
        settings_.get_int(QStringLiteral("clipboard.max_entries"), 200);
    if (max_entries.is_ok()) {
        const Result<int> trim_result = clipboard_.enforce_retention(max_entries.value(), 0);
        if (trim_result.is_ok() && trim_result.value() > 0) {
            emit entries_trimmed(trim_result.value());
        }
    }

    emit entry_recorded(insert_result.value());
}

} // namespace quickdeck
