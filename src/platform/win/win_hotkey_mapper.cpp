#include "platform/win/win_hotkey_mapper.h"

#include <QKeyCombination>

namespace quickdeck {

namespace {

constexpr quint32 kModAlt = 0x0001;
constexpr quint32 kModControl = 0x0002;
constexpr quint32 kModShift = 0x0004;
constexpr quint32 kModWin = 0x0008;
constexpr quint32 kModNoRepeat = 0x4000;

[[nodiscard]] quint32 modifiers_to_win32(Qt::KeyboardModifiers modifiers)
{
    quint32 result = kModNoRepeat;
    if (modifiers.testFlag(Qt::AltModifier)) {
        result |= kModAlt;
    }
    if (modifiers.testFlag(Qt::ControlModifier)) {
        result |= kModControl;
    }
    if (modifiers.testFlag(Qt::ShiftModifier)) {
        result |= kModShift;
    }
    if (modifiers.testFlag(Qt::MetaModifier)) {
        result |= kModWin;
    }
    return result;
}

[[nodiscard]] Result<quint32> key_to_vk(int key)
{
    if (key >= Qt::Key_A && key <= Qt::Key_Z) {
        return Result<quint32>::ok(static_cast<quint32>(key));
    }
    if (key >= Qt::Key_0 && key <= Qt::Key_9) {
        return Result<quint32>::ok(static_cast<quint32>(key));
    }
    if (key >= Qt::Key_F1 && key <= Qt::Key_F24) {
        return Result<quint32>::ok(0x70 + static_cast<quint32>(key - Qt::Key_F1));
    }

    switch (key) {
    case Qt::Key_Space:
        return Result<quint32>::ok(0x20);
    case Qt::Key_Tab:
        return Result<quint32>::ok(0x09);
    case Qt::Key_Backspace:
        return Result<quint32>::ok(0x08);
    case Qt::Key_Escape:
        return Result<quint32>::ok(0x1B);
    default:
        break;
    }
    return Result<quint32>::fail(
        QStringLiteral("Unsupported hotkey key: %1").arg(key));
}

} // namespace

bool is_supported_hotkey_key(int qt_key)
{
    return key_to_vk(qt_key).is_ok();
}

Result<WinHotkeyParts> parse_qkey_sequence(const QKeySequence &sequence)
{
    if (sequence.isEmpty() || sequence.count() != 1) {
        return Result<WinHotkeyParts>::fail(
            QStringLiteral("Hotkey must be a single key combination"));
    }

    const QKeyCombination combo = QKeyCombination::fromCombined(sequence[0].toCombined());
    const int key = combo.key();
    if (key == 0 || key == Qt::Key_unknown) {
        return Result<WinHotkeyParts>::fail(QStringLiteral("Hotkey has no key"));
    }

    const Qt::KeyboardModifiers modifiers = combo.keyboardModifiers();
    if (modifiers == Qt::NoModifier) {
        return Result<WinHotkeyParts>::fail(
            QStringLiteral("Global hotkeys require at least one modifier"));
    }

    const Result<quint32> vk = key_to_vk(key);
    if (vk.is_err()) {
        return Result<WinHotkeyParts>::fail(vk.error());
    }

    WinHotkeyParts parts;
    parts.modifiers = modifiers_to_win32(modifiers);
    parts.vk = vk.value();
    return Result<WinHotkeyParts>::ok(parts);
}

} // namespace quickdeck
