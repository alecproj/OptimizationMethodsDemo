//
// Created on 29 Nov, 2025
//  by alecproj
//

#ifndef SOURCES_SOLUTIONTABLEMODEL_HPP_
#define SOURCES_SOLUTIONTABLEMODEL_HPP_

#include <QAbstractTableModel>
#include <QJsonArray>
#include <QVector>
#include <QVariant>
#include <QJsonObject>
#include <QHash>

class SolutionTableModel : public QAbstractTableModel {
    Q_OBJECT

    Q_PROPERTY(QStringList headers READ headers NOTIFY headersChanged)
    Q_PROPERTY(int columnCount READ columnCount NOTIFY columnCountChanged)

public:
    explicit SolutionTableModel(QObject *parent = nullptr);

    // Basic model interface (QAbstractTableModel)
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // optional roleNames (not required for TableView which uses DisplayRole)
    QHash<int, QByteArray> roleNames() const override;

    // JSON loading API
    Q_INVOKABLE void loadFromJsonObject(const QJsonObject &obj); // load full object { "columns": [...], "rows": [...] }
    Q_INVOKABLE void loadFromJsonArrayChunked(const QJsonArray &arr, int chunkSize = 1000); // load chunked

    // lazy fetch support
    bool canFetchMore(const QModelIndex &parent) const override;
    void fetchMore(const QModelIndex &parent) override;

    // getters
    QStringList headers() const { return m_headers; }

signals:
    void headersChanged();
    void columnCountChanged();

private:
    void rebuildColumnCount();

private:
    QStringList m_headers;
    QVector<QVector<QVariant>> m_rows;

    // chunked source (expects format: [ { "columns":[...], "rows":[ [...], ... ] } ] )
    QJsonArray m_sourceArray;
    int m_nextLoadIndex = 0;
    int m_chunkSize = 1000;

    int m_columnCount = 0;
};

#endif // SOURCES_SOLUTIONTABLEMODEL_HPP_
