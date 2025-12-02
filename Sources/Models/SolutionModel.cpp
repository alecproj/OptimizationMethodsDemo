//
// Created on 8 Nov, 2025
//  by alecproj
//

#include "SolutionModel.hpp"
#include "SolutionMessage.hpp"
#include "SolutionValue.hpp"
#include "SolutionTable.hpp"

#include <QVariant>

SolutionModel::SolutionModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_items{}
{
}

int SolutionModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    return m_items.size();
}

QVariant SolutionModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) return {};
    if (index.row() < 0 || index.row() >= m_items.size()) return {};
    auto entry = m_items.at(index.row());

    switch (role) {
        case EntryRole:
            return QVariant::fromValue<QObject*>(entry);
        default:
            return {};
    }
}

QHash<int, QByteArray> SolutionModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[EntryRole] = "entry";
    return roles;
}

void SolutionModel::setData(const QJsonArray &arr)
{
    beginResetModel();
    m_items.clear();
    endResetModel();

    for (const auto item : arr) {
        QJsonObject obj = item.toObject();
        QString type = obj.value("type").toString();

        if (QString::compare(type, "message") == 0) {
            QString text = obj.value("text").toString();
            auto entry = new SolutionMessage(text, this);
            m_items.append(entry);
        } else if (QString::compare(type, "value") == 0) {
            QString title = obj.value("title").toString();
            double value = obj.value("value").toDouble();
            auto entry = new SolutionValue(value, title, this);
            m_items.append(entry);
        } else if (QString::compare(type, "table") == 0) {
            QString title = obj.value("title").toString();
            auto table = new SolutionTableModel();
            table->loadFromJsonObject(obj);
            auto entry = new SolutionTable(table, title, this);
            table->setParent(entry);
            m_items.append(entry);
        } else {
            qWarning() << "Unknown type received: " << type;
        }
    }
    beginResetModel();
    endResetModel();
}
