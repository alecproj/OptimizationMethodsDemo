//
// Created on 28 Nov, 2025
//  by alecproj
//

#ifndef SOURCES_GOINPUTDATA_HPP_
#define SOURCES_GOINPUTDATA_HPP_

#include "InputData.hpp"
#include "AppEnums.hpp"
#include <QObject>
#include <QString>

namespace GO {

class InputData : public ::InputData
{
    Q_OBJECT

    // string
    Q_PROPERTY(QString function READ function WRITE setFunction NOTIFY functionChanged)

    // int
    Q_PROPERTY(int extremumId READ extremumId WRITE setExtremumId NOTIFY extremumIdChanged)
    Q_PROPERTY(int calcAccuracy READ calcAccuracy WRITE setCalcAccuracy NOTIFY calcAccuracyChanged)
    Q_PROPERTY(int resultAccuracy READ resultAccuracy WRITE setResultAccuracy NOTIFY resultAccuracyChanged)

    // double
    Q_PROPERTY(double minX READ minX WRITE setMinX NOTIFY minXChanged)
    Q_PROPERTY(double maxX READ maxX WRITE setMaxX NOTIFY maxXChanged)
    Q_PROPERTY(double minY READ minY WRITE setMinY NOTIFY minYChanged)
    Q_PROPERTY(double maxY READ maxY WRITE setMaxY NOTIFY maxYChanged)

public:
    explicit InputData(QObject* parent = nullptr)
        : ::InputData(parent)
        , m_function()
        , m_algorithmId(AlgoType::CD)
        , m_extremumId(ExtremumType::MINIMUM)
        , m_calcAccuracy(0)
        , m_resultAccuracy(0)
        , m_minX(0.0)
        , m_maxX(0.0)
        , m_minY(0.0)
        , m_maxY(0.0)
    {}

    // Getters
    PartType::Type partitionId() const override { return PartType::GO; }
    AlgoType::Type algorithmId() const override { return m_algorithmId; }

    QString function() const { return m_function; }

    ExtremumType::Type extremumId() const { return m_extremumId; }
    int calcAccuracy() const { return m_calcAccuracy; }
    int resultAccuracy() const { return m_resultAccuracy; }

    double minX() const { return m_minX; }
    double maxX() const { return m_maxX; }
    double minY() const { return m_minY; }
    double maxY() const { return m_maxY; }

    Q_INVOKABLE InputData *instance() { return this; }

    Q_INVOKABLE QString calcAccuracyAsString() const { return QString::number(m_calcAccuracy); }
    Q_INVOKABLE QString resultAccuracyAsString() const { return QString::number(m_resultAccuracy); }

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

    Q_INVOKABLE QString minXAsString() const { return formatDoubleWithDot(m_minX); }
    Q_INVOKABLE QString maxXAsString() const { return formatDoubleWithDot(m_maxX); }
    Q_INVOKABLE QString minYAsString() const { return formatDoubleWithDot(m_minY); }
    Q_INVOKABLE QString maxYAsString() const { return formatDoubleWithDot(m_maxY); }

public slots:
    // Setters
    void setAlgorithmId(int v) override
    {
        auto value = static_cast<AlgoType::Type>(v);
        if (m_algorithmId != value) {
            m_algorithmId = value;
            emit algorithmIdChanged();
        }
    }
    void setFunction(const QString& v)
    {
        if (m_function != v) {
            m_function = v;
            emit functionChanged();
        }
    }
    void setExtremumId(int v)
    {
        auto value = static_cast<ExtremumType::Type>(v);
        if (m_extremumId != value) {
            m_extremumId = value;
            emit extremumIdChanged();
        }
    }
    void setCalcAccuracy(int v)
    {
        if (m_calcAccuracy != v) {
            m_calcAccuracy = v;
            emit calcAccuracyChanged();
        }
    }
    void setResultAccuracy(int v)
    {
        if (m_resultAccuracy != v) {
            m_resultAccuracy = v;
            emit resultAccuracyChanged();
        }
    }
    void setMinX(double v)
    {
        if (!qFuzzyCompare(m_minX, v)) {
            m_minX = v;
            emit minXChanged();
        }
    }
    void setMaxX(double v)
    {
        if (!qFuzzyCompare(m_maxX, v)) {
            m_maxX = v;
            emit maxXChanged();
        }
    }
    void setMinY(double v)
    {
        if (!qFuzzyCompare(m_minY, v)) {
            m_minY = v;
            emit minYChanged();
        }
    }
    void setMaxY(double v)
    {
        if (!qFuzzyCompare(m_maxY, v)) {
            m_maxY = v;
            emit maxYChanged();
        }
    }

public:
    // Q_INVOKABLE helpers: set value from QString. Return true if conversion succeeded and value was set.

    Q_INVOKABLE bool setCalcAccuracyFromString(const QString& s)
    {
        bool rv = false;
        int v = s.toInt(&rv);
        if (rv) {
            setCalcAccuracy(v);
        }
        return rv;
    }
    Q_INVOKABLE bool setResultAccuracyFromString(const QString& s)
    {
        bool rv = false;
        int v = s.toInt(&rv);
        if (rv) {
            setResultAccuracy(v);
        }
        return rv;
    }
    Q_INVOKABLE bool setMinXFromString(const QString& s)
    {
        bool rv = false;
        double v = s.toDouble(&rv);
        if (rv) {
            setMinX(v);
        }
        return rv;
    }
    Q_INVOKABLE bool setMaxXFromString(const QString& s)
    {
        bool rv = false;
        double v = s.toDouble(&rv);
        if (rv) {
            setMaxX(v);
        }
        return rv;
    }
    Q_INVOKABLE bool setMinYFromString(const QString& s)
    {
        bool rv = false;
        double v = s.toDouble(&rv);
        if (rv) {
            setMinY(v);
        }
        return rv;
    }
    Q_INVOKABLE bool setMaxYFromString(const QString& s)
    {
        bool rv = false;
        double v = s.toDouble(&rv);
        if (rv) {
            setMaxY(v);
        }
        return rv;
    }

signals:
    void functionChanged();

    void extremumIdChanged();
    void calcAccuracyChanged();
    void resultAccuracyChanged();

    void minXChanged();
    void maxXChanged();
    void minYChanged();
    void maxYChanged();

private:
    AlgoType::Type m_algorithmId;

    QString m_function;

    ExtremumType::Type m_extremumId;
    int m_calcAccuracy;
    int m_resultAccuracy;

    double m_minX;
    double m_maxX;
    double m_minY;
    double m_maxY;
};

} // namespace GO

#endif // SOURCES_GOINPUTDATA_HPP_
