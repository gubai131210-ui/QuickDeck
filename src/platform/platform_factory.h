#pragma once

#include "core/interfaces/i_platform_services.h"

#include <memory>

namespace quickdeck {

[[nodiscard]] std::unique_ptr<IPlatformServices> create_platform_services();

} // namespace quickdeck
