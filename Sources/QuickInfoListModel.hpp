//
// Created on 4 Nov, 2025
//  by alecproj
//

#ifndef SOURCES_QUICKINFOLISTMODEL_HPP_
#define SOURCES_QUICKINFOLISTMODEL_HPP_

#include "QuickInfo.hpp"
#include <QAbstractListModel>

class QuickInfoListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    using ObjectList = QList<QuickInfo *>;
    using RoleNames = QHash<int, QByteArray>;

    explicit QuickInfoListModel(QObject *parent = nullptr)
        : QAbstractListModel{parent}
        , m_data{}
    {
    }

    enum Roles {
        PartitionRole = Qt::UserRole + 1,
        NameRole,
        InfoRole,
        StatusRole,
    };

    // Basic functionality
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    RoleNames roleNames() const override;

    void beginTraversing();
    bool mark(const QString &fileName);
    void endTraversing();

    bool exists(const QString &fileName) const;
    void prepend(QuickInfo *entry) 
    {
        beginInsertRows(QModelIndex(), 0, 0);
        m_data.prepend(entry);
        endInsertRows();
    }
    void clear()
    {
        beginResetModel();
        m_data.clear();
        endResetModel();
    }
    void deleteEntry(const QString &fileName);
    int count() const { return m_data.count(); }

private:
    QList<QuickInfo *> m_data;

};

#endif // SOURCES_QUICKINFOLISTMODEL_HPP_
