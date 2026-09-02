#include "ui/models/clipboard_history_model.h"

namespace quickdeck {

ClipboardHistoryModel::ClipboardHistoryModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int ClipboardHistoryModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return entries_.size();
}

QVariant ClipboardHistoryModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= entries_.size()) {
        return {};
    }
    const ClipboardEntry &entry = entries_.at(index.row());
    switch (role) {
    case IdRole:
        return entry.id;
    case PreviewRole:
        return entry.preview;
    case ContentRole:
        return entry.content;
    case CreatedAtRole:
        return entry.created_at;
    case IsPinnedRole:
        return entry.is_pinned;
    default:
        return {};
    }
}

QHash<int, QByteArray> ClipboardHistoryModel::roleNames() const
{
    return {
        {IdRole, "entryId"},
        {PreviewRole, "preview"},
        {ContentRole, "content"},
        {CreatedAtRole, "createdAt"},
        {IsPinnedRole, "isPinned"},
    };
}

void ClipboardHistoryModel::set_entries(const QList<ClipboardEntry> &entries)
{
    beginResetModel();
    entries_ = entries;
    endResetModel();
}

} // namespace quickdeck
