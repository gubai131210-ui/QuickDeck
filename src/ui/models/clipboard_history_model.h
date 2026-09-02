#pragma once

#include "core/domain/types.h"

#include <QAbstractListModel>

namespace quickdeck {

class ClipboardHistoryModel : public QAbstractListModel {
    Q_OBJECT

public:
    enum Roles {
        IdRole = Qt::UserRole + 1,
        PreviewRole,
        ContentRole,
        CreatedAtRole,
        IsPinnedRole
    };
    Q_ENUM(Roles)

    explicit ClipboardHistoryModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    void set_entries(const QList<ClipboardEntry> &entries);

private:
    QList<ClipboardEntry> entries_;
};

} // namespace quickdeck
