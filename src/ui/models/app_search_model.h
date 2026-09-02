#pragma once

#include "core/domain/types.h"

#include <QAbstractListModel>

namespace quickdeck {

class AppSearchModel : public QAbstractListModel {
    Q_OBJECT

public:
    enum Roles {
        IdRole = Qt::UserRole + 1,
        NameRole,
        PathRole,
        SubtitleRole,
        IconRole,
        IsPinnedRole
    };
    Q_ENUM(Roles)

    explicit AppSearchModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    void set_entries(const QList<AppEntry> &entries);

private:
    QList<AppEntry> entries_;
};

} // namespace quickdeck
