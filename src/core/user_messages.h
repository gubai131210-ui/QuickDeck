#pragma once

#include <QString>

namespace quickdeck {

namespace ErrorCodes {
inline constexpr char kLaunchFileMissing[] = "launch.file_missing";
inline constexpr char kLaunchStartFailed[] = "launch.start_failed";
inline constexpr char kOpenPathFailed[] = "open.path_failed";
inline constexpr char kPathNotFound[] = "path.not_found";
inline constexpr char kIndexScanFailed[] = "index.scan_failed";
inline constexpr char kIndexUpsertFailed[] = "index.upsert_failed";
} // namespace ErrorCodes

class UserMessages {
public:
    [[nodiscard]] static QString translate_error(const QString &error_code);
    [[nodiscard]] static bool is_error_code(const QString &value);
};

} // namespace quickdeck
