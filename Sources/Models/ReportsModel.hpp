//
// Created on 28 Nov, 2025
//  by alecproj
//

#ifndef SOURCES_REPORTSMODEL_HPP_
#define SOURCES_REPORTSMODEL_HPP_

#include "Report.hpp"
#include <QAbstractListModel>

class ReportsModel : public QAbstractListModel {
    Q_OBJECT
public:
    enum Roles {
        PartitionRole = Qt::UserRole + 1,
        FileNameRole,
        InputDataRole,
        SolutionRole,
        ResultDataRole
    };

    ReportsModel(QObject *parent = nullptr) : QAbstractListModel(parent) {}

    int rowCount(const QModelIndex &parent = {}) const override {
        if (parent.isValid()) return 0;
        return m_reports.size();
    }

    QVariant data(const QModelIndex &index, int role) const override {
        if (!index.isValid()) {
            return {};
        }
        auto report = m_reports.at(index.row());
        switch (role) {
            case PartitionRole:
                return report->partition();
            case FileNameRole:
                return report->fileName();
            case InputDataRole:
                return QVariant::fromValue<QObject*>(report->inputData());
            case SolutionRole:
                return QVariant::fromValue<QObject*>(report->solution());
            case ResultDataRole:
                return QVariant::fromValue<QObject*>(report->resultData());
            default:
                return {};
        }
    }

    QHash<int, QByteArray> roleNames() const override {
        QHash<int, QByteArray> roles;
        roles[PartitionRole] = "partition";
        roles[FileNameRole] = "fileName";
        roles[InputDataRole] = "inputData";
        roles[SolutionRole] = "solution";
        roles[ResultDataRole] = "resultData";
        return roles;
    }

    void appendReport(Report *report) {
        beginInsertRows({}, m_reports.size(), m_reports.size());
        m_reports.append(report);
        endInsertRows();
    }

    bool indexOf(const QString &fileName, size_t &outIdx) const
    {
        const size_t reportsCnt = m_reports.count();
        for (size_t i = 0; i < reportsCnt; ++i) {
            if (m_reports[i]->fileName() == fileName) {
                outIdx = i;
                return true;
            }
        }
        return false;
    }

    void removeReport(const QString &fileName)
    {
        const size_t reportsCnt = m_reports.count();
        for (size_t i = 0; i < reportsCnt; ++i) {
            if (m_reports[i]->fileName() == fileName) {
                beginRemoveRows({}, i, i);
                m_reports.remove(i, 1);
                endRemoveRows();
                return;
            }
        }
        qCritical() << "Attempting to close an unopened report";
    }

    auto countReports()
    {
        return m_reports.count();
    }

private:
    QVector<Report*> m_reports;
};

#endif // SOURCES_REPORTSMODEL_HPP_
