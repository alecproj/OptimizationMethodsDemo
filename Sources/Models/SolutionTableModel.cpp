//
// Created on 29 Nov, 2025
//  by alecproj
//

#include "SolutionTableModel.hpp"
#include <QJsonValue>
#include <QJsonArray>
#include <QDebug>

SolutionTableModel::SolutionTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

int SolutionTableModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid()) return 0;
    return static_cast<int>(m_rows.size());
}

int SolutionTableModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return m_columnCount;
}

QVariant SolutionTableModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) return QVariant();
    int r = index.row();
    int c = index.column();
    if (r < 0 || r >= m_rows.size()) return QVariant();
    if (c < 0 || c >= m_rows[r].size()) return QVariant();

    if (role == Qt::DisplayRole) {
        return m_rows[r][c];
    }

    return QVariant();
}

QVariant SolutionTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        if (section >= 0 && section < m_headers.size()) {
            return m_headers.at(section);
        }
        // fallback
        return QString("col%1").arg(section);
    }
    if (orientation == Qt::Vertical && role == Qt::DisplayRole) {
        return section + 1; // row numbers (optional)
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}

QHash<int, QByteArray> SolutionTableModel::roleNames() const {
    // TableView uses DisplayRole; no custom roles required.
    // Still returning an empty map is OK. If you want roles, you can map Qt::DisplayRole here,
    // but typically roleNames() is used for list-based role access in QML.
    return { {Qt::DisplayRole, "display"} };
}

void SolutionTableModel::rebuildColumnCount() {
    int newCount = 0;
    if (!m_headers.isEmpty()) newCount = m_headers.size();
    else if (!m_rows.isEmpty()) {
        // determine max row length to be safe
        for (const auto &r : m_rows) if (r.size() > newCount) newCount = r.size();
    } else newCount = 0;

    if (newCount != m_columnCount) {
        m_columnCount = newCount;
        emit columnCountChanged();
    }
}

void SolutionTableModel::loadFromJsonObject(const QJsonObject &obj) {
    beginResetModel();

    m_rows.clear();
    m_headers.clear();
    m_sourceArray = QJsonArray();
    m_nextLoadIndex = 0;
    m_chunkSize = 1000;
    m_columnCount = 0;

    if (obj.isEmpty()) {
        qCritical() << "Empty object";
        endResetModel();
        emit headersChanged();
        return;
    }

    if (obj.contains("columns")) {
        QJsonArray cols = obj.value("columns").toArray();
        for (const QJsonValue &v : cols) m_headers.append(v.toString());
    }

    if (obj.contains("rows")) {
        QJsonArray rows = obj.value("rows").toArray();
        m_rows.reserve(rows.size());
        for (const QJsonValue &rv : rows) {
            QJsonArray cells = rv.toArray();
            QVector<QVariant> row;
            row.reserve(cells.size());
            for (const QJsonValue &cv : cells) row.push_back(cv.toVariant());
            m_rows.push_back(std::move(row));
        }
    }

    rebuildColumnCount();

    endResetModel();
    emit headersChanged();
    qDebug() << "Table has loaded";
}

void SolutionTableModel::loadFromJsonArrayChunked(const QJsonArray &arr, int chunkSize) {
    beginResetModel();

    m_rows.clear();
    m_headers.clear();
    m_sourceArray = arr;
    m_nextLoadIndex = 0;
    m_chunkSize = qMax(1, chunkSize);
    m_columnCount = 0;

    endResetModel();

    if (arr.isEmpty()) {
        emit headersChanged();
        return;
    }

    QJsonValue first = arr.first();
    if (first.isObject()) {
        QJsonObject obj = first.toObject();
        if (obj.contains("columns")) {
            QJsonArray cols = obj.value("columns").toArray();
            for (const QJsonValue &v : cols) m_headers.append(v.toString());
            rebuildColumnCount();
        }
    }

    emit headersChanged();

    // load first chunk if available
    if (canFetchMore(QModelIndex()))
        fetchMore(QModelIndex());
}

bool SolutionTableModel::canFetchMore(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    if (m_sourceArray.isEmpty()) return false;
    QJsonValue first = m_sourceArray.first();
    if (!first.isObject()) return false;
    QJsonObject obj = first.toObject();
    if (!obj.contains("rows")) return false;
    QJsonArray rows = obj.value("rows").toArray();
    return m_nextLoadIndex < rows.size();
}

void SolutionTableModel::fetchMore(const QModelIndex &parent) {
    Q_UNUSED(parent);
    if (m_sourceArray.isEmpty()) return;
    QJsonValue first = m_sourceArray.first();
    if (!first.isObject()) return;
    QJsonObject obj = first.toObject();
    if (!obj.contains("rows")) return;

    QJsonArray rows = obj.value("rows").toArray();
    int remaining = rows.size() - m_nextLoadIndex;
    int itemsToLoad = qMin(remaining, m_chunkSize);
    if (itemsToLoad <= 0) return;

    beginInsertRows(QModelIndex(), m_rows.size(), m_rows.size() + itemsToLoad - 1);
    for (int i = 0; i < itemsToLoad; ++i) {
        QJsonArray cells = rows.at(m_nextLoadIndex++).toArray();
        QVector<QVariant> row;
        row.reserve(cells.size());
        for (const QJsonValue &cv : cells) row.push_back(cv.toVariant());
        m_rows.push_back(std::move(row));
    }

    // if column count was unknown, infer now
    rebuildColumnCount();

    endInsertRows();
}
