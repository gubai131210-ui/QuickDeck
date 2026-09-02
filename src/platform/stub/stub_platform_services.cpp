#include "platform/stub/stub_platform_services.h"

namespace quickdeck {

Result<void> StubPlatformServices::register_hotkey(const QString &,
                                                   const QKeySequence &,
                                                   std::function<void()>)
{
    return Result<void>::fail(QStringLiteral("Hotkeys not supported on this platform"));
}

Result<void> StubPlatformServices::unregister_hotkey(const QString &)
{
    return Result<void>::ok();
}

Result<bool> StubPlatformServices::is_hotkey_available(const QKeySequence &)
{
    return Result<bool>::ok(false);
}

Result<void> StubPlatformServices::launch_app(const AppEntry &, bool)
{
    return Result<void>::fail(QStringLiteral("Launch not supported on this platform"));
}

Result<void> StubPlatformServices::open_path(const QString &)
{
    return Result<void>::fail(QStringLiteral("Open path not supported on this platform"));
}

Result<void> StubPlatformServices::reveal_in_file_manager(const QString &)
{
    return Result<void>::fail(QStringLiteral("Reveal not supported on this platform"));
}

Result<void> StubPlatformServices::simulate_paste()
{
    return Result<void>::fail(QStringLiteral("Simulated paste not supported on this platform"));
}

Result<void> StubPlatformServices::set_auto_start_enabled(bool)
{
    return Result<void>::ok();
}

Result<bool> StubPlatformServices::is_auto_start_enabled()
{
    return Result<bool>::ok(false);
}

QString StubPlatformServices::platform_id() const
{
    return QStringLiteral("stub");
}

} // namespace quickdeck
