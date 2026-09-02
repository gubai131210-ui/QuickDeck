#pragma once

#include "core/domain/types.h"
#include "core/result.h"

#include <QString>

namespace quickdeck {

class IconCache {
public:
    [[nodiscard]] static QString cache_directory();
    [[nodiscard]] static Result<QString> cache_icon_for_entry(const AppEntry &entry);
};

} // namespace quickdeck
