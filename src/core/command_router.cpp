#include "core/command_router.h"

#include <QStringList>

namespace quickdeck {

namespace {

struct BuiltinCommand {
    QString id;
    QStringList keywords;
    QString title;
    QString subtitle;
    QString action_hint;
};

const QVector<BuiltinCommand> &builtins()
{
    static const QVector<BuiltinCommand> commands = {
        {QStringLiteral("settings"),
         {QStringLiteral("settings"), QStringLiteral("setting"), QStringLiteral("set")},
         QStringLiteral("Open Settings"),
         QStringLiteral("Configure QuickDeck preferences"),
         QStringLiteral("Enter")},
        {QStringLiteral("lang"),
         {QStringLiteral("lang"), QStringLiteral("language")},
         QStringLiteral("Switch Language"),
         QStringLiteral("Use >lang en or >lang zh"),
         QStringLiteral("Enter")},
        {QStringLiteral("refresh"),
         {QStringLiteral("refresh"), QStringLiteral("reindex"), QStringLiteral("index")},
         QStringLiteral("Refresh Application Index"),
         QStringLiteral("Rescan installed applications"),
         QStringLiteral("Enter")},
        {QStringLiteral("search"),
         {QStringLiteral("search"), QStringLiteral("launcher")},
         QStringLiteral("Switch to Search Mode"),
         QStringLiteral("Find and launch applications"),
         QStringLiteral("Enter")},
        {QStringLiteral("clipboard"),
         {QStringLiteral("clipboard"), QStringLiteral("clip"), QStringLiteral("history")},
         QStringLiteral("Switch to Clipboard Mode"),
         QStringLiteral("Browse clipboard history"),
         QStringLiteral("Enter")},
        {QStringLiteral("paste"),
         {QStringLiteral("paste"), QStringLiteral("quickpaste")},
         QStringLiteral("Quick Paste Latest"),
         QStringLiteral("Paste the most recent clipboard entry"),
         QStringLiteral("Enter")},
    };
    return commands;
}

bool keyword_matches(const BuiltinCommand &command, const QString &token)
{
    if (token.isEmpty()) {
        return true;
    }
    for (const QString &keyword : command.keywords) {
        if (keyword.startsWith(token) || keyword.contains(token)) {
            return true;
        }
    }
    return false;
}

CommandItem make_item(const BuiltinCommand &command, const QString &argument)
{
    CommandItem item;
    item.id = command.id;
    item.title = command.title;
    item.subtitle = command.subtitle;
    item.action_hint = command.action_hint;
    item.argument = argument;

    if (command.id == QStringLiteral("lang") && !argument.isEmpty()) {
        item.subtitle = QStringLiteral("Switch to %1").arg(argument);
    }
    return item;
}

} // namespace

bool CommandRouter::is_command_query(const QString &query)
{
    return query.trimmed().startsWith(QLatin1Char('>'));
}

QVector<CommandItem> CommandRouter::match_commands(const QString &query)
{
    if (!is_command_query(query)) {
        return {};
    }

    const QString rest = query.trimmed().mid(1).trimmed();
    QString token = rest;
    QString argument;
    const int space_index = rest.indexOf(QLatin1Char(' '));
    if (space_index >= 0) {
        token = rest.left(space_index).trimmed().toLower();
        argument = rest.mid(space_index + 1).trimmed();
    } else {
        token = token.toLower();
    }

    QVector<CommandItem> matches;
    for (const BuiltinCommand &command : builtins()) {
        if (!keyword_matches(command, token)) {
            continue;
        }
        if (command.id == QStringLiteral("lang") && !argument.isEmpty()) {
            matches.append(make_item(command, argument));
            continue;
        }
        if (command.id == QStringLiteral("lang") && token == QStringLiteral("lang")) {
            matches.append(make_item(command, argument));
            matches.append(make_item(command, QStringLiteral("en")));
            matches.append(make_item(command, QStringLiteral("zh")));
            continue;
        }
        matches.append(make_item(command, argument));
    }
    return matches;
}

} // namespace quickdeck
