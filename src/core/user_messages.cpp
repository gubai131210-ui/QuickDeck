#include "core/user_messages.h"

#include <QCoreApplication>

namespace quickdeck {

bool UserMessages::is_error_code(const QString &value)
{
    return value.contains(QLatin1Char('.')) && !value.contains(QLatin1Char(' '));
}

QString UserMessages::translate_error(const QString &error_code)
{
    if (error_code == QLatin1String(ErrorCodes::kLaunchFileMissing)) {
        return QCoreApplication::translate("UserMessages",
                                           "The application file was not found.");
    }
    if (error_code == QLatin1String(ErrorCodes::kLaunchStartFailed)) {
        return QCoreApplication::translate("UserMessages", "The application could not be started.");
    }
    if (error_code == QLatin1String(ErrorCodes::kOpenPathFailed)) {
        return QCoreApplication::translate("UserMessages", "The path could not be opened.");
    }
    if (error_code == QLatin1String(ErrorCodes::kPathNotFound)) {
        return QCoreApplication::translate("UserMessages", "The path does not exist.");
    }
    if (error_code == QLatin1String(ErrorCodes::kIndexScanFailed)) {
        return QCoreApplication::translate("UserMessages",
                                           "Could not scan installed applications.");
    }
    if (error_code == QLatin1String(ErrorCodes::kIndexUpsertFailed)) {
        return QCoreApplication::translate("UserMessages", "Could not save the application index.");
    }
    return error_code;
}

} // namespace quickdeck
