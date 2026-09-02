#pragma once

#include "core/command_router.h"

#include <QAbstractListModel>

namespace quickdeck {

class CommandSearchModel : public QAbstractListModel {
    Q_OBJECT

public:
    enum Roles {
        IdRole = Qt::UserRole + 1,
        TitleRole,
        SubtitleRole,
        ActionHintRole,
        ArgumentRole
    };
    Q_ENUM(Roles)

    explicit CommandSearchModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    void set_entries(const QVector<CommandItem> &entries);

private:
    QVector<CommandItem> entries_;
};

} // namespace quickdeck
