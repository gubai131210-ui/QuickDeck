#include "platform/stub/stub_platform_services.h"

#include "platform/win/win_hotkey_mapper.h"

namespace quickdeck {

void StubPlatformServices::block_hotkey(const QKeySequence &sequence)
{
    blocked_.insert(sequence.toString(QKeySequence::PortableText));
}

Result<void> StubPlatformServices::register_hotkey(const QString &id,
                                                   const QKeySequence &sequence,
                                                   std::function<void()> callback)
{
    const Result<WinHotkeyParts> parts = parse_qkey_sequence(sequence);
    if (parts.is_err()) {
        return Result<void>::fail(parts.error());
    }

    registered_.insert(id, sequence);
    callbacks_.insert(id, std::move(callback));
    return Result<void>::ok();
}

Result<void> StubPlatformServices::unregister_hotkey(const QString &id)
{
    registered_.remove(id);
    callbacks_.remove(id);
    return Result<void>::ok();
}

Result<bool> StubPlatformServices::is_hotkey_available(const QKeySequence &sequence)
{
    const Result<WinHotkeyParts> parts = parse_qkey_sequence(sequence);
    if (parts.is_err()) {
        return Result<bool>::fail(parts.error());
    }

    const QString portable = sequence.toString(QKeySequence::PortableText);
    return Result<bool>::ok(!blocked_.contains(portable));
}

Result<void> StubPlatformServices::launch_app(const AppEntry &, bool)
{
    return Result<void>::ok();
}

Result<void> StubPlatformServices::open_path(const QString &)
{
    return Result<void>::ok();
}

Result<void> StubPlatformServices::reveal_in_file_manager(const QString &)
{
    return Result<void>::ok();
}

Result<void> StubPlatformServices::simulate_paste()
{
    return Result<void>::fail(QStringLiteral("Simulated paste not supported"));
}

Result<void> StubPlatformServices::set_auto_start_enabled(bool enabled)
{
    auto_start_ = enabled;
    return Result<void>::ok();
}

Result<bool> StubPlatformServices::is_auto_start_enabled()
{
    return Result<bool>::ok(auto_start_);
}

QString StubPlatformServices::platform_id() const
{
    return QStringLiteral("stub");
}

QHash<QString, QKeySequence> StubPlatformServices::registered_hotkeys() const
{
    return registered_;
}

} // namespace quickdeck
