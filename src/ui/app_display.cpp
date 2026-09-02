#include "ui/app_display.h"

#ifdef Q_OS_WIN
#include "platform/win/win_lnk_resolver.h"
#endif

namespace quickdeck {

QString app_entry_subtitle(const AppEntry &entry)
{
#ifdef Q_OS_WIN
    if (entry.executable_path.endsWith(QStringLiteral(".lnk"), Qt::CaseInsensitive)) {
        const Result<QString> target = resolve_lnk_target(entry.executable_path);
        if (target.is_ok()) {
            return target.value();
        }
    }
#endif
    return entry.executable_path;
}

} // namespace quickdeck
