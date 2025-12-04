//
// Created on 3 Nov, 2025
//  by alecproj
//

#ifndef SOURCES_INPUTDATA_HPP_
#define SOURCES_INPUTDATA_HPP_

#include "AppEnums.hpp"
#include <QObject>
#include <QString>

#define DECL_QPROP_INT(PROP, PROP_CAP)                                      \
private:                                                                    \
    int m_##PROP = 0;                                                       \
public:                                                                     \
    Q_PROPERTY(int PROP READ PROP WRITE set##PROP_CAP NOTIFY PROP##Changed) \
    int PROP() const { return m_##PROP; }                                   \
    void set##PROP_CAP(int value) {                                         \
        if (m_##PROP != value) {                                            \
            m_##PROP = value;                                               \
            emit PROP##Changed();                                           \
        }                                                                   \
    }                                                                       \
    Q_INVOKABLE QString PROP##AsString() const {                            \
        return QString::number(m_##PROP);                                   \
    }                                                                       \
    Q_INVOKABLE bool set##PROP_CAP##FromString(const QString &s) {          \
        bool ok = false;                                                    \
        int v = s.toInt(&ok);                                               \
        if (ok) set##PROP_CAP(v);                                           \
        return ok;                                                          \
    }                                                                       \
Q_SIGNALS:                                                                  \
    void PROP##Changed();

// Макрос для double
#define DECL_QPROP_DOUBLE(PROP, PROP_CAP)                                   \
private:                                                                    \
    double m_##PROP = 0.0;                                                  \
public:                                                                     \
    Q_PROPERTY(double PROP READ PROP WRITE set##PROP_CAP NOTIFY PROP##Changed) \
    double PROP() const { return m_##PROP; }                                \
    void set##PROP_CAP(double value) {                                      \
        if (qFuzzyCompare(m_##PROP, value)) return;                         \
        m_##PROP = value;                                                   \
        emit PROP##Changed();                                               \
    }                                                                       \
    Q_INVOKABLE QString PROP##AsString() const {                            \
        return formatDoubleWithDot(m_##PROP);                               \
    }                                                                       \
    Q_INVOKABLE bool set##PROP_CAP##FromString(const QString &s) {          \
        bool ok = false;                                                    \
        double v = s.toDouble(&ok);                                         \
        if (ok) set##PROP_CAP(v);                                           \
        return ok;                                                          \
    }                                                                       \
Q_SIGNALS:                                                                  \
    void PROP##Changed();


class InputData : public QObject {
    Q_OBJECT
    Q_PROPERTY(int partitionId READ partitionId CONSTANT)
    Q_PROPERTY(int algorithmId READ algorithmId WRITE setAlgorithmId NOTIFY algorithmIdChanged)

public:
    explicit InputData(QObject *parent = nullptr) : QObject(parent) {}
    virtual PartType::Type partitionId() const = 0;
    virtual AlgoType::Type algorithmId() const = 0;

    static QString formatDoubleWithDot(double v) {
        QString s = QString::number(v, 'f', 15);
        if (s.contains('.')) {
            while (s.endsWith('0'))
                s.chop(1);
            if (s.endsWith('.'))
                s += '0';
        } else {
            s += ".0";
        }
        return s;
    }

public slots:
    virtual void setAlgorithmId(int v) = 0;

signals:
    void algorithmIdChanged();
};

#endif // SOURCES_INPUTDATA_HPP_
