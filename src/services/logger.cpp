#include "services/logger.h"

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QStandardPaths>
#include <QTextStream>

namespace quickdeck {

Logger &Logger::instance()
{
    static Logger logger;
    return logger;
}

void Logger::set_debug_enabled(bool enabled)
{
    debug_enabled_ = enabled;
}

void Logger::log(LogLevel level, const QString &message)
{
    if (level == LogLevel::Debug && !debug_enabled_) {
        return;
    }

    QString level_text;
    switch (level) {
    case LogLevel::Debug:
        level_text = QStringLiteral("DEBUG");
        break;
    case LogLevel::Info:
        level_text = QStringLiteral("INFO");
        break;
    case LogLevel::Warning:
        level_text = QStringLiteral("WARN");
        break;
    case LogLevel::Error:
        level_text = QStringLiteral("ERROR");
        break;
    }

    const QString line = QStringLiteral("[%1] [%2] %3")
                             .arg(QDateTime::currentDateTime().toString(Qt::ISODate), level_text,
                                  message);
    write_to_file(line);
}

void Logger::write_to_file(const QString &line)
{
    const QString log_dir =
        QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + QStringLiteral("/logs");
    QDir().mkpath(log_dir);

    const QString log_path = log_dir + QStringLiteral("/quickdeck.log");
    QFileInfo log_info(log_path);
    if (log_info.exists() && log_info.size() > 5 * 1024 * 1024) {
        const QString rotated_path = log_dir + QStringLiteral("/quickdeck.log.1");
        QFile::remove(rotated_path);
        QFile::rename(log_path, rotated_path);
    }

    QFile file(log_path);
    if (file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        QTextStream stream(&file);
        stream << line << '\n';
    }
}

} // namespace quickdeck
