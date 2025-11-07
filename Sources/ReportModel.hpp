//
// Created on 6 Nov, 2025
//  by alecproj
//

#ifndef SOURCES_REPORTMODEL_HPP_
#define SOURCES_REPORTMODEL_HPP_

#include <QAbstractListModel>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>

class ReportModel : public QAbstractListModel {
    Q_OBJECT
public:
    QString m_fileName;

    enum Roles {
        TypeRole = Qt::UserRole + 1,
        TitleRole,
        ValueRole,
        TextRole,
        ColumnsRole,
        RowsRole
    };

    ReportModel(QObject *parent = nullptr);

    // QAbstractListModel
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    void setData(const QJsonArray &arr, const QString &filename);

private:
    QJsonArray m_items;
};

#endif // SOURCES_REPORTMODEL_HPP_
