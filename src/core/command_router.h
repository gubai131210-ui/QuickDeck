#pragma once

#include <QString>
#include <QVector>

namespace quickdeck {

struct CommandItem {
    QString id;
    QString title;
    QString subtitle;
    QString action_hint;
    QString argument;
};

class CommandRouter {
public:
    [[nodiscard]] static QVector<CommandItem> match_commands(const QString &query);
    [[nodiscard]] static bool is_command_query(const QString &query);
};

} // namespace quickdeck
