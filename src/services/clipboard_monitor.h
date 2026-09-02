#pragma once

#include "core/interfaces/repositories.h"
#include "core/result.h"

#include <QObject>

namespace quickdeck {

class ClipboardMonitor : public QObject {
    Q_OBJECT

public:
    ClipboardMonitor(IClipboardRepository &clipboard, ISettingsStore &settings,
                     QObject *parent = nullptr);

    Result<void> start();
    void stop();

    void set_suppress_next_change(bool suppress);

signals:
    void entry_recorded(qint64 entry_id);
    void monitoring_paused_changed(bool paused);

private:
    void on_clipboard_changed();

    IClipboardRepository &clipboard_;
    ISettingsStore &settings_;
    bool suppress_next_change_ = false;
    bool running_ = false;
};

} // namespace quickdeck
