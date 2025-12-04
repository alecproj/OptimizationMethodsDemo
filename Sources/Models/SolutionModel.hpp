//
// Created on 8 Nov, 2025
//  by alecproj
//

#ifndef SOURCES_SOLUTIONMODEL_HPP_
#define SOURCES_SOLUTIONMODEL_HPP_

#include "SolutionEntry.hpp"
#include <QAbstractListModel>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>

class SolutionModel : public QAbstractListModel {
    Q_OBJECT
public:
    enum Roles {
        EntryRole = Qt::UserRole + 1,
    };

    SolutionModel(QObject *parent = nullptr);

    // QAbstractListModel
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    void setData(const QJsonArray &arr);

private:
    QList<SolutionEntry *> m_items;
};

#endif // SOURCES_SOLUTIONMODEL_HPP_
