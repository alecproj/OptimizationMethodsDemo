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

    // Common for GO
    Q_PROPERTY(QString function READ function WRITE setFunction NOTIFY functionChanged)
    Q_PROPERTY(int extremumId READ extremumId WRITE setExtremumId NOTIFY extremumIdChanged)

    DECL_QPROP_INT(calcAccuracy, CalcAccuracy)
    DECL_QPROP_INT(resultAccuracy, ResultAccuracy)
    DECL_QPROP_INT(size, Size)
    DECL_QPROP_INT(maxIterations, MaxIterations)
    DECL_QPROP_DOUBLE(minX, MinX)
    DECL_QPROP_DOUBLE(maxX, MaxX)
    DECL_QPROP_DOUBLE(minY, MinY)
    DECL_QPROP_DOUBLE(maxY, MaxY)

    // Genetic Algorithm specific
    DECL_QPROP_INT(elitism, Elitism)
    DECL_QPROP_DOUBLE(crossoverProb, CrossoverProb)
    DECL_QPROP_DOUBLE(mutationProb, MutationProb)

    // Particle Swarm specific
    DECL_QPROP_DOUBLE(inertiaCoef, InertiaCoef)
    DECL_QPROP_DOUBLE(cognitiveCoef, CognitiveCoef)
    DECL_QPROP_DOUBLE(socialCoef, SocialCoef)

public:
    explicit InputData(QObject* parent = nullptr)
        : ::InputData(parent)
    {
    }

    // Getters
    PartType::Type partitionId() const override { return PartType::GO; }
    AlgoType::Type algorithmId() const override { return m_algorithmId; }

    QString function() const { return m_function; }
    ExtremumType::Type extremumId() const { return m_extremumId; }

    Q_INVOKABLE InputData *instance() { return this; }

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

signals:
    void functionChanged();
    void extremumIdChanged();

private:
    AlgoType::Type m_algorithmId;

    QString m_function;
    ExtremumType::Type m_extremumId;
};

} // namespace GO

#endif // SOURCES_GOINPUTDATA_HPP_
