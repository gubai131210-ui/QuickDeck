#pragma once

#include "core/domain/types.h"
#include "core/result.h"

#include <QKeySequence>
#include <functional>

namespace quickdeck {

struct HotkeyRegistration {
    QKeySequence sequence;
    std::function<void()> callback;
};

class IPlatformServices {
public:
    virtual ~IPlatformServices() = default;

    virtual Result<void> register_hotkey(const QString &id,
                                         const QKeySequence &sequence,
                                         std::function<void()> callback) = 0;
    virtual Result<void> unregister_hotkey(const QString &id) = 0;
    virtual Result<bool> is_hotkey_available(const QKeySequence &sequence) = 0;

    virtual Result<void> launch_app(const AppEntry &app, bool as_admin = false) = 0;
    virtual Result<void> open_path(const QString &path) = 0;
    virtual Result<void> reveal_in_file_manager(const QString &path) = 0;

    virtual Result<void> simulate_paste() = 0;

    virtual Result<void> set_auto_start_enabled(bool enabled) = 0;
    virtual Result<bool> is_auto_start_enabled() = 0;

    virtual QString platform_id() const = 0;
};

} // namespace quickdeck
