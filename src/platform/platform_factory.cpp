#include "platform/platform_factory.h"

#ifdef Q_OS_WIN
#include "platform/win/win_platform_services.h"
#else
#include "platform/stub/stub_platform_services.h"
#endif

namespace quickdeck {

std::unique_ptr<IPlatformServices> create_platform_services()
{
#ifdef Q_OS_WIN
    return std::make_unique<WinPlatformServices>();
#else
    return std::make_unique<StubPlatformServices>();
#endif
}

} // namespace quickdeck
