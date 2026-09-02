#include "ui/models/app_search_model.h"

namespace quickdeck {

AppSearchModel::AppSearchModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int AppSearchModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return entries_.size();
}

QVariant AppSearchModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= entries_.size()) {
        return {};
    }
    const AppEntry &entry = entries_.at(index.row());
    switch (role) {
    case IdRole:
        return entry.id;
    case NameRole:
        return entry.name;
    case PathRole:
        return entry.executable_path;
    case IconRole:
        return entry.icon_cache_path;
    case IsPinnedRole:
        return entry.is_pinned;
    default:
        return {};
    }
}

QHash<int, QByteArray> AppSearchModel::roleNames() const
{
    return {
        {IdRole, "entryId"},
        {NameRole, "name"},
        {PathRole, "path"},
        {IconRole, "iconPath"},
        {IsPinnedRole, "isPinned"},
    };
}

void AppSearchModel::set_entries(const QList<AppEntry> &entries)
{
    beginResetModel();
    entries_ = entries;
    endResetModel();
}

} // namespace quickdeck
