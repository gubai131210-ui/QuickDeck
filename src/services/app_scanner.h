#pragma once

#include "core/domain/types.h"
#include "core/result.h"

#include <QList>
#include <QString>

namespace quickdeck {

[[nodiscard]] Result<QList<AppEntry>> scan_lnk_roots(const QStringList &roots);

} // namespace quickdeck
