#include "platform/win/win_platform_services.h"

#include "core/user_messages.h"
#include "platform/win/win_hotkey_mapper.h"
#include "services/logger.h"

#include <QCoreApplication>
#include <QDesktopServices>
#include <QDir>
#include <QFileInfo>
#include <QMetaObject>
#include <QProcess>
#include <QSettings>
#include <QUrl>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

namespace quickdeck {

WinPlatformServices::WinPlatformServices()
{
    QCoreApplication::instance()->installNativeEventFilter(this);
}

WinPlatformServices::~WinPlatformServices()
{
    const QStringList ids = hotkeys_.keys();
    for (const QString &id : ids) {
        unregister_hotkey(id);
    }
    QCoreApplication::instance()->removeNativeEventFilter(this);
}

Result<void> WinPlatformServices::register_hotkey(const QString &id,
                                                  const QKeySequence &sequence,
                                                  std::function<void()> callback)
{
#ifdef Q_OS_WIN
    if (hotkeys_.contains(id)) {
        unregister_hotkey(id);
    }

    const Result<WinHotkeyParts> parts = parse_qkey_sequence(sequence);
    if (parts.is_err()) {
        return Result<void>::fail(parts.error());
    }

    const int native_id = next_hotkey_id_++;
    if (!RegisterHotKey(nullptr, native_id, parts.value().modifiers, parts.value().vk)) {
        const DWORD error = GetLastError();
        return Result<void>::fail(
            QStringLiteral("Failed to register hotkey %1 (Win32 error %2)")
                .arg(id)
                .arg(error));
    }

    QD_LOG_INFO(QStringLiteral("Hotkey registered: %1 %2 mods=0x%3 vk=0x%4")
                    .arg(id, sequence.toString(QKeySequence::PortableText))
                    .arg(parts.value().modifiers, 0, 16)
                    .arg(parts.value().vk, 0, 16));

    hotkeys_.insert(id, HotkeyEntry{native_id, std::move(callback)});
    native_id_to_key_.insert(native_id, id);
    return Result<void>::ok();
#else
    Q_UNUSED(id)
    Q_UNUSED(sequence)
    Q_UNUSED(callback)
    return Result<void>::fail(QStringLiteral("Not Windows"));
#endif
}

Result<void> WinPlatformServices::unregister_hotkey(const QString &id)
{
#ifdef Q_OS_WIN
    if (!hotkeys_.contains(id)) {
        return Result<void>::ok();
    }
    const HotkeyEntry entry = hotkeys_.take(id);
    native_id_to_key_.remove(entry.native_id);
    UnregisterHotKey(nullptr, entry.native_id);
    return Result<void>::ok();
#else
    Q_UNUSED(id)
    return Result<void>::ok();
#endif
}

Result<bool> WinPlatformServices::is_hotkey_available(const QKeySequence &sequence)
{
#ifdef Q_OS_WIN
    const Result<WinHotkeyParts> parts = parse_qkey_sequence(sequence);
    if (parts.is_err()) {
        return Result<bool>::fail(parts.error());
    }

    const int probe_id = next_hotkey_id_;
    const bool ok = RegisterHotKey(nullptr, probe_id, parts.value().modifiers, parts.value().vk);
    if (ok) {
        UnregisterHotKey(nullptr, probe_id);
    }
    return Result<bool>::ok(ok);
#else
    Q_UNUSED(sequence)
    return Result<bool>::ok(false);
#endif
}

Result<void> WinPlatformServices::launch_app(const AppEntry &app, bool as_admin)
{
    QString executable = app.executable_path;
    if (executable.endsWith(QStringLiteral(".lnk"), Qt::CaseInsensitive)) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(executable));
        return Result<void>::ok();
    }

    QStringList arguments;
    if (!app.launch_args.isEmpty()) {
        arguments = app.launch_args.split(QLatin1Char(' '), Qt::SkipEmptyParts);
    }
    const QString working_dir = app.working_dir;

    if (as_admin) {
#ifdef Q_OS_WIN
        QProcess elevated;
        elevated.start(QStringLiteral("powershell"),
                       {QStringLiteral("-Command"),
                        QStringLiteral("Start-Process -FilePath '%1' -Verb RunAs").arg(executable)});
        elevated.waitForFinished(3000);
        return Result<void>::ok();
#endif
    }

    if (!QFileInfo::exists(executable)) {
        return Result<void>::fail(QString::fromLatin1(ErrorCodes::kLaunchFileMissing));
    }

    if (!QProcess::startDetached(executable, arguments,
                                 working_dir.isEmpty() ? QString() : working_dir)) {
        return Result<void>::fail(QString::fromLatin1(ErrorCodes::kLaunchStartFailed));
    }
    return Result<void>::ok();
}

Result<void> WinPlatformServices::open_path(const QString &path)
{
    if (!QDesktopServices::openUrl(QUrl::fromLocalFile(path))) {
        return Result<void>::fail(QString::fromLatin1(ErrorCodes::kOpenPathFailed));
    }
    return Result<void>::ok();
}

Result<void> WinPlatformServices::reveal_in_file_manager(const QString &path)
{
    const QFileInfo info(path);
    const QString target = info.isDir() ? path : info.absolutePath();
#ifdef Q_OS_WIN
    QProcess::startDetached(QStringLiteral("explorer.exe"),
                            {QStringLiteral("/select,"), QDir::toNativeSeparators(path)});
#else
    QDesktopServices::openUrl(QUrl::fromLocalFile(target));
#endif
    Q_UNUSED(target)
    return Result<void>::ok();
}

Result<void> WinPlatformServices::simulate_paste()
{
#ifdef Q_OS_WIN
    INPUT inputs[4] = {};
    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = VK_CONTROL;
    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = 0x56;
    inputs[2].type = INPUT_KEYBOARD;
    inputs[2].ki.wVk = 0x56;
    inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;
    inputs[3].type = INPUT_KEYBOARD;
    inputs[3].ki.wVk = VK_CONTROL;
    inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(4, inputs, sizeof(INPUT));
    return Result<void>::ok();
#else
    return Result<void>::fail(QStringLiteral("Simulated paste not supported"));
#endif
}

Result<void> WinPlatformServices::set_auto_start_enabled(bool enabled)
{
    QSettings settings(QStringLiteral("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run"),
                       QSettings::NativeFormat);
    if (enabled) {
        settings.setValue(QStringLiteral("QuickDeck"),
                          QDir::toNativeSeparators(QCoreApplication::applicationFilePath()));
    } else {
        settings.remove(QStringLiteral("QuickDeck"));
    }
    return Result<void>::ok();
}

Result<bool> WinPlatformServices::is_auto_start_enabled()
{
    QSettings settings(QStringLiteral("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run"),
                       QSettings::NativeFormat);
    return Result<bool>::ok(settings.contains(QStringLiteral("QuickDeck")));
}

QString WinPlatformServices::platform_id() const
{
    return QStringLiteral("win");
}

bool WinPlatformServices::nativeEventFilter(const QByteArray &event_type,
                                            void *message,
                                            qintptr *result)
{
#ifdef Q_OS_WIN
    if (event_type == "windows_dispatcher_MSG" || event_type == "windows_generic_MSG") {
        MSG *msg = static_cast<MSG *>(message);
        if (msg->message == WM_HOTKEY) {
            const int native_id = static_cast<int>(msg->wParam);
            if (native_id_to_key_.contains(native_id)) {
                const QString key = native_id_to_key_.value(native_id);
                if (hotkeys_.contains(key) && hotkeys_.value(key).callback) {
                    const std::function<void()> callback = hotkeys_.value(key).callback;
                    QMetaObject::invokeMethod(
                        QCoreApplication::instance(),
                        [callback]() { callback(); },
                        Qt::QueuedConnection);
                }
                if (result != nullptr) {
                    *result = 0;
                }
                return true;
            }
        }
    }
#else
    Q_UNUSED(event_type)
    Q_UNUSED(message)
    Q_UNUSED(result)
#endif
    return false;
}

} // namespace quickdeck
