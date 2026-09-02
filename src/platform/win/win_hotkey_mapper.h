#pragma once

#include "core/result.h"

#include <QKeySequence>

namespace quickdeck {

// Win32 MOD_* / VK_* values stored as quint32 for cross-platform unit tests.
struct WinHotkeyParts {
    quint32 modifiers = 0x4000; // MOD_NOREPEAT
    quint32 vk = 0;
};

[[nodiscard]] Result<WinHotkeyParts> parse_qkey_sequence(const QKeySequence &sequence);
[[nodiscard]] bool is_supported_hotkey_key(int qt_key);

} // namespace quickdeck
