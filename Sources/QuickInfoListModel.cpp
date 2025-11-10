//
// Created on 4 Nov, 2025
//  by alecproj
//

#include "QuickInfoListModel.hpp"

int QuickInfoListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_data.size();
}

QVariant QuickInfoListModel::data(const QModelIndex &index, int role) const
{
    if (!checkIndex(index, CheckIndexOption::IndexIsValid))
        return QVariant();

    if (!index.isValid())
        return QVariant();

    const auto &obj = m_data.at(index.row());
    switch (role) {
        case NameRole: return obj->name();
        case InfoRole: return obj->info();
        case StatusRole: return obj->status();
        default: return QVariant();
    }
}

QHash<int, QByteArray> QuickInfoListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[InfoRole] = "info";
    roles[StatusRole] = "status";
    return roles;
}

bool QuickInfoListModel::exists(const QString &fileName) const
{
    for (const auto entry : m_data) {
        if (QString::compare(entry->name(), fileName) == 0) {
            return true;
        }
    }
    return false;
}

void QuickInfoListModel::deleteEntry(const QString &fileName)
{
    const int len = m_data.count();
    for (int i = 0; i < len; ++i) {
        if (QString::compare(m_data[i]->name(), fileName) == 0) {
            beginRemoveRows(QModelIndex(), i, i);
            m_data.remove(i);
            endRemoveRows();
            return;
        }
    }
}
