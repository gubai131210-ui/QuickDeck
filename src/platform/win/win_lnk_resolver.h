#pragma once

#include "core/result.h"

class QString;

namespace quickdeck {

[[nodiscard]] Result<QString> resolve_lnk_target(const QString &lnk_path);

} // namespace quickdeck
