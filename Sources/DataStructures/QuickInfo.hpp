//
// Created on 4 Nov, 2025
//  by alecproj
//

#ifndef SOURCES_QUICKINFO_HPP_
#define SOURCES_QUICKINFO_HPP_

#include "AppEnums.hpp"

#include <QObject>
#include <QString>

class QuickInfo: public QObject
{
    Q_OBJECT

    Q_PROPERTY(int partition READ partition WRITE setPartition NOTIFY partitionChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString info READ info WRITE setInfo NOTIFY infoChanged)
    Q_PROPERTY(int status READ status WRITE setStatus NOTIFY statusChanged)
public:
    bool marked;

    explicit QuickInfo(QObject* parent = nullptr)
        : QObject(parent)
        , marked{false}
        , m_partition{PartType::NONE}
        , m_name{}
        , m_info{}
        , m_status{ReportStatus::NotVerified}
    {}

    // Getters
    PartType::Type partition() const { return m_partition; }
    QString name() const { return m_name; }
    QString info() const { return m_info; }
    ReportStatus::Status status() const { return m_status; }

    Q_INVOKABLE QuickInfo *instance() { return this; }

public slots:
    // Setters
    void setPartition(int v)
    {
        auto value = static_cast<PartType::Type>(v);
        if (m_partition != value) {
            m_partition = value;
            emit partitionChanged();
        }
    }
    void setName(const QString& v)
    {
        if (m_name != v) {
            m_name = v;
            emit nameChanged();
        }
    }
    void setInfo(const QString& v)
    {
        if (m_info != v) {
            m_info = v;
            emit infoChanged();
        }
    }
    void setStatus(int v)
    {
        auto value = static_cast<ReportStatus::Status>(v);
        if (m_status != value) {
            m_status = value;
            emit statusChanged();
        }
    }

signals:
    void partitionChanged();
    void nameChanged();
    void infoChanged();
    void statusChanged();

private:
    PartType::Type m_partition;
    QString m_name;
    QString m_info;
    ReportStatus::Status m_status;

};
#endif // SOURCES_QUICKINFO_HPP_
