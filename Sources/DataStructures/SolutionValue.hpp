//
// Created on 29 Nov, 2025
//  by alecproj
//

#ifndef SOURCES_SOLUTIONVALUE_HPP_
#define SOURCES_SOLUTIONVALUE_HPP_

#include "SolutionMessage.hpp"
#include <QObject>
#include <QString>

class SolutionValue : public SolutionMessage {
    Q_OBJECT
    Q_PROPERTY(double value READ value CONSTANT)

public:
    explicit SolutionValue(double value, const QString &text, QObject *parent = nullptr)
        : SolutionMessage(text, parent)
        , m_value(value)
    {
    }

    QString type() const override { return "value"; };
    double value() const { return m_value; }
private:
    const double m_value;
};

#endif // SOURCES_SOLUTIONVALUE_HPP_
