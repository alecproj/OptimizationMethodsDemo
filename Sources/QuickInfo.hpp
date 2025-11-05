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

    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString info READ info WRITE setInfo NOTIFY infoChanged)
    Q_PROPERTY(int status READ status WRITE setStatus NOTIFY statusChanged)
public:

    explicit QuickInfo(QObject* parent = nullptr)
        : QObject(parent)
        , m_name{}
        , m_info{}
        , m_status{ReportStatus::NotVerified}
    {}

    // Getters
    QString name() const { return m_name; }
    QString info() const { return m_info; }
    ReportStatus::Status status() const { return m_status; }

    Q_INVOKABLE QuickInfo *instance() { return this; }

public slots:
    // Setters
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
    void nameChanged();
    void infoChanged();
    void statusChanged();

private:
    QString m_name;
    QString m_info;
    ReportStatus::Status m_status;

};
#endif // SOURCES_QUICKINFO_HPP_
