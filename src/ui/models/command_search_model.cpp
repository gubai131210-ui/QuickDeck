#include "ui/models/command_search_model.h"

namespace quickdeck {

CommandSearchModel::CommandSearchModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int CommandSearchModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return entries_.size();
}

QVariant CommandSearchModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= entries_.size()) {
        return {};
    }

    const CommandItem &entry = entries_.at(index.row());
    switch (role) {
    case IdRole:
        return entry.id;
    case TitleRole:
        return entry.title;
    case SubtitleRole:
        return entry.subtitle;
    case ActionHintRole:
        return entry.action_hint;
    case ArgumentRole:
        return entry.argument;
    default:
        return {};
    }
}

QHash<int, QByteArray> CommandSearchModel::roleNames() const
{
    return {
        {IdRole, "commandId"},
        {TitleRole, "title"},
        {SubtitleRole, "subtitle"},
        {ActionHintRole, "actionHint"},
        {ArgumentRole, "argument"},
    };
}

void CommandSearchModel::set_entries(const QVector<CommandItem> &entries)
{
    beginResetModel();
    entries_ = entries;
    endResetModel();
}

} // namespace quickdeck
