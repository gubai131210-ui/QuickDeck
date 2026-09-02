#pragma once

#include <QString>

namespace quickdeck {

enum class LogLevel {
    Debug,
    Info,
    Warning,
    Error
};

class Logger {
public:
    static Logger &instance();

    void set_debug_enabled(bool enabled);
    void log(LogLevel level, const QString &message);

private:
    Logger() = default;
    void write_to_file(const QString &line);

    bool debug_enabled_ = false;
};

} // namespace quickdeck

#define QD_LOG_INFO(msg) quickdeck::Logger::instance().log(quickdeck::LogLevel::Info, (msg))
#define QD_LOG_WARN(msg) quickdeck::Logger::instance().log(quickdeck::LogLevel::Warning, (msg))
#define QD_LOG_ERROR(msg) quickdeck::Logger::instance().log(quickdeck::LogLevel::Error, (msg))
#define QD_LOG_DEBUG(msg) quickdeck::Logger::instance().log(quickdeck::LogLevel::Debug, (msg))
