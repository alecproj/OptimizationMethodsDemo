//
// Created on 6 Nov, 2025
//  by alecproj
//

#include "ReportModel.hpp"

#include <QVariant>

ReportModel::ReportModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_fileName{}
    , m_items{}
{
}

int ReportModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    return m_items.size();
}

QVariant ReportModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) return {};
    if (index.row() < 0 || index.row() >= m_items.size()) return {};
    QJsonObject obj = m_items.at(index.row()).toObject();

    switch (role) {
    case TypeRole: return obj.value("type").toString();
    case TitleRole: return obj.value("title").toString();
    case ValueRole: return obj.value("value").toDouble();
    case TextRole: return obj.value("text").toString();
    case ColumnsRole: {
        QJsonArray cols = obj.value("columns").toArray();
        QStringList list;
        for (const QJsonValue &v : cols) list << v.toString();
        return list;
    }
    case RowsRole: {
        QJsonArray rows = obj.value("rows").toArray();
        QVariantList out;
        for (const QJsonValue &r : rows) {
            QJsonArray row = r.toArray();
            QVariantList rowList;
            for (const QJsonValue &cell : row) rowList << cell.toVariant();
            out << QVariant::fromValue(rowList);
        }
        return out;
    }
    default:
        return {};
    }
}

QHash<int, QByteArray> ReportModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[TypeRole] = "type";
    roles[TitleRole] = "title";
    roles[ValueRole] = "value";
    roles[TextRole] = "text";
    roles[ColumnsRole] = "columns";
    roles[RowsRole] = "rows";
    return roles;
}

void ReportModel::setData(const QJsonArray &arr, const QString &fileName)
{
    m_fileName = fileName;
    beginResetModel();
    m_items = arr;
    endResetModel();
}
