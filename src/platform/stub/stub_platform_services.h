#pragma once

#include "core/interfaces/i_platform_services.h"

namespace quickdeck {

class StubPlatformServices final : public IPlatformServices {
public:
    Result<void> register_hotkey(const QString &id,
                                 const QKeySequence &sequence,
                                 std::function<void()> callback) override;
    Result<void> unregister_hotkey(const QString &id) override;
    Result<bool> is_hotkey_available(const QKeySequence &sequence) override;

    Result<void> launch_app(const AppEntry &app, bool as_admin) override;
    Result<void> open_path(const QString &path) override;
    Result<void> reveal_in_file_manager(const QString &path) override;

    Result<void> simulate_paste() override;

    Result<void> set_auto_start_enabled(bool enabled) override;
    Result<bool> is_auto_start_enabled() override;

    QString platform_id() const override;
};

} // namespace quickdeck
