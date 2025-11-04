//
// Created on 3 Nov, 2025
//  by alecproj
//

#ifndef SOURCES_INPUTDATA_HPP_
#define SOURCES_INPUTDATA_HPP_

#include "AppEnums.hpp"
#include <QObject>
#include <QString>

class InputData : public QObject
{
    Q_OBJECT

    // string
    Q_PROPERTY(QString function READ function WRITE setFunction NOTIFY functionChanged)

    // int
    Q_PROPERTY(int algorithmId READ algorithmId WRITE setAlgorithmId NOTIFY algorithmIdChanged)
    Q_PROPERTY(int extensionId READ extensionId WRITE setExtensionId NOTIFY extensionIdChanged)
    Q_PROPERTY(int fullAlgoId READ fullAlgoId WRITE setFullAlgoId NOTIFY fullAlgoIdChanged)
    Q_PROPERTY(int extremumId READ extremumId WRITE setExtremumId NOTIFY extremumIdChanged)
    Q_PROPERTY(int maxIterations READ maxIterations WRITE setMaxIterations NOTIFY maxIterationsChanged)

    // double
    Q_PROPERTY(double calcAccuracy READ calcAccuracy WRITE setCalcAccuracy NOTIFY calcAccuracyChanged)
    Q_PROPERTY(double resultAccuracy READ resultAccuracy WRITE setResultAccuracy NOTIFY resultAccuracyChanged)
    Q_PROPERTY(double startX1 READ startX1 WRITE setStartX1 NOTIFY startX1Changed)
    Q_PROPERTY(double startY1 READ startY1 WRITE setStartY1 NOTIFY startY1Changed)
    Q_PROPERTY(double startX2 READ startX2 WRITE setStartX2 NOTIFY startX2Changed)
    Q_PROPERTY(double startY2 READ startY2 WRITE setStartY2 NOTIFY startY2Changed)
    Q_PROPERTY(double stepX READ stepX WRITE setStepX NOTIFY stepXChanged)
    Q_PROPERTY(double stepY READ stepY WRITE setStepY NOTIFY stepYChanged)
    Q_PROPERTY(double coefficientStep READ coefficientStep WRITE setCoefficientStep NOTIFY coefficientStepChanged)
    Q_PROPERTY(double minX READ minX WRITE setMinX NOTIFY minXChanged)
    Q_PROPERTY(double maxX READ maxX WRITE setMaxX NOTIFY maxXChanged)
    Q_PROPERTY(double minY READ minY WRITE setMinY NOTIFY minYChanged)
    Q_PROPERTY(double maxY READ maxY WRITE setMaxY NOTIFY maxYChanged)

public:
    explicit InputData(QObject* parent = nullptr)
        : QObject(parent)
        , m_function()
        , m_algorithmId(AlgoType::CD)
        , m_extensionId(ExtensionType::B)
        , m_fullAlgoId(FullAlgoType::CDB)
        , m_extremumId(ExtremumType::MINIMUM)
        , m_maxIterations(0)
        , m_calcAccuracy(0.0)
        , m_resultAccuracy(0.0)
        , m_startX1(0.0)
        , m_startY1(0.0)
        , m_startX2(0.0)
        , m_startY2(0.0)
        , m_stepX(0.0)
        , m_stepY(0.0)
        , m_coefficientStep(0.0)
        , m_minX(0.0)
        , m_maxX(0.0)
        , m_minY(0.0)
        , m_maxY(0.0)
    {}

    // Getters
    QString function() const { return m_function; }

    AlgoType::Type algorithmId() const { return m_algorithmId; }
    ExtensionType::Type extensionId() const { return m_extensionId; }
    FullAlgoType::Type fullAlgoId() const { return m_fullAlgoId; }
    ExtremumType::Type extremumId() const { return m_extremumId; }
    int maxIterations() const { return m_maxIterations; }

    double calcAccuracy() const { return m_calcAccuracy; }
    double resultAccuracy() const { return m_resultAccuracy; }
    double startX1() const { return m_startX1; }
    double startY1() const { return m_startY1; }
    double startX2() const { return m_startX2; }
    double startY2() const { return m_startY2; }
    double stepX() const { return m_stepX; }
    double stepY() const { return m_stepY; }
    double coefficientStep() const { return m_coefficientStep; }
    double minX() const { return m_minX; }
    double maxX() const { return m_maxX; }
    double minY() const { return m_minY; }
    double maxY() const { return m_maxY; }

    Q_INVOKABLE InputData *instance() { return this; }

public slots:
    // Setters
    void setFunction(const QString& v)
    {
        if (m_function != v) {
            m_function = v;
            emit functionChanged();
        }
    }

    void setAlgorithmId(int v)
    {
        auto value = static_cast<AlgoType::Type>(v);
        if (m_algorithmId != value) {
            m_algorithmId = value;
            emit algorithmIdChanged();
        }
    }
    void setExtensionId(int v)
    {
        auto value = static_cast<ExtensionType::Type>(v);
        if (m_extensionId != value) {
            m_extensionId = value;
            emit extensionIdChanged();
        }
    }
    void setFullAlgoId(int v)
    {
        auto value = static_cast<FullAlgoType::Type>(v);
        if (m_fullAlgoId != value) {
            m_fullAlgoId = value;
            emit fullAlgoIdChanged();
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
    void setMaxIterations(int v)
    {
        if (m_maxIterations != v) {
            m_maxIterations = v;
            emit maxIterationsChanged();
        }
    }

    void setCalcAccuracy(double v)
    {
        if (!qFuzzyCompare(m_calcAccuracy, v)) {
            m_calcAccuracy = v;
            emit calcAccuracyChanged();
        }
    }
    void setResultAccuracy(double v)
    {
        if (!qFuzzyCompare(m_resultAccuracy, v)) {
            m_resultAccuracy = v;
            emit resultAccuracyChanged();
        }
    }
    void setStartX1(double v)
    {
        if (!qFuzzyCompare(m_startX1, v)) {
            m_startX1 = v;
            emit startX1Changed();
        }
    }
    void setStartY1(double v)
    {
        if (!qFuzzyCompare(m_startY1, v)) {
            m_startY1 = v;
            emit startY1Changed();
        }
    }
    void setStartX2(double v)
    {
        if (!qFuzzyCompare(m_startX2, v)) {
            m_startX2 = v;
            emit startX2Changed();
        }
    }
    void setStartY2(double v)
    {
        if (!qFuzzyCompare(m_startY2, v)) {
            m_startY2 = v;
            emit startY2Changed();
        }
    }
    void setStepX(double v)
    {
        if (!qFuzzyCompare(m_stepX, v)) {
            m_stepX = v;
            emit stepXChanged();
        }
    }
    void setStepY(double v)
    {
        if (!qFuzzyCompare(m_stepY, v)) {
            m_stepY = v;
            emit stepYChanged();
        }
    }
    void setCoefficientStep(double v)
    {
        if (!qFuzzyCompare(m_coefficientStep, v)) {
            m_coefficientStep = v;
            emit coefficientStepChanged();
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

    Q_INVOKABLE bool setMaxIterationsFromString(const QString& s)
    {
        bool ok = false;
        int v = s.toInt(&ok);
        if (ok) {
            setMaxIterations(v);
        }
        return ok;
    }

    Q_INVOKABLE bool setCalcAccuracyFromString(const QString& s)
    {
        bool rv = false;
        double v = s.toDouble(&rv);
        if (rv) {
            setCalcAccuracy(v);
        }
        return rv;
    }
    Q_INVOKABLE bool setResultAccuracyFromString(const QString& s)
    {
        bool rv = false;
        double v = s.toDouble(&rv);
        if (rv) {
            setResultAccuracy(v);
        }
        return rv;
    }
    Q_INVOKABLE bool setStartX1FromString(const QString& s)
    {
        bool rv = false;
        double v = s.toDouble(&rv);
        if (rv) {
            setStartX1(v);
        }
        return rv;
    }
    Q_INVOKABLE bool setStartY1FromString(const QString& s)
    {
        bool rv = false;
        double v = s.toDouble(&rv);
        if (rv) {
            setStartY1(v);
        }
        return rv;
    }
    Q_INVOKABLE bool setStartX2FromString(const QString& s)
    {
        bool rv = false;
        double v = s.toDouble(&rv);
        if (rv) {
            setStartX2(v);
        }
        return rv;
    }
    Q_INVOKABLE bool setStartY2FromString(const QString& s)
    {
        bool rv = false;
        double v = s.toDouble(&rv);
        if (rv) {
            setStartY2(v);
        }
        return rv;
    }
    Q_INVOKABLE bool setStepXFromString(const QString& s)
    {
        bool rv = false;
        double v = s.toDouble(&rv);
        if (rv) {
            setStepX(v);
        }
        return rv;
    }
    Q_INVOKABLE bool setStepYFromString(const QString& s)
    {
        bool rv = false;
        double v = s.toDouble(&rv);
        if (rv) {
            setStepY(v);
        }
        return rv;
    }
    Q_INVOKABLE bool setCoefficientStepFromString(const QString& s)
    {
        bool rv = false;
        double v = s.toDouble(&rv);
        if (rv) {
            setCoefficientStep(v);
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

    void algorithmIdChanged();
    void extensionIdChanged();
    void fullAlgoIdChanged();
    void extremumIdChanged();
    void maxIterationsChanged();

    void calcAccuracyChanged();
    void resultAccuracyChanged();
    void startX1Changed();
    void startY1Changed();
    void startX2Changed();
    void startY2Changed();
    void stepXChanged();
    void stepYChanged();
    void coefficientStepChanged();
    void minXChanged();
    void maxXChanged();
    void minYChanged();
    void maxYChanged();

private:
    QString m_function;

    AlgoType::Type m_algorithmId;
    ExtensionType::Type m_extensionId;
    FullAlgoType::Type m_fullAlgoId;
    ExtremumType::Type m_extremumId;
    int m_maxIterations;

    double m_calcAccuracy;
    double m_resultAccuracy;
    double m_startX1;
    double m_startY1;
    double m_startX2;
    double m_startY2;
    double m_stepX;
    double m_stepY;
    double m_coefficientStep;
    double m_minX;
    double m_maxX;
    double m_minY;
    double m_maxY;
};

#endif // SOURCES_INPUTDATA_HPP_
