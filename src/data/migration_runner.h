#pragma once

#include "core/result.h"

#include <QObject>
#include <QSqlDatabase>
#include <QString>

namespace quickdeck {

class MigrationRunner {
public:
    static Result<void> apply_all(QSqlDatabase &db, const QString &migrations_dir);

private:
    static Result<int> current_version(QSqlDatabase &db);
    static Result<void> apply_file(QSqlDatabase &db, const QString &file_path);
};

} // namespace quickdeck
