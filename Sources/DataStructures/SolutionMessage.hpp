//
// Created on 29 Nov, 2025
//  by alecproj
//

#ifndef SOURCES_SOLUTIONMESSAGE_HPP_
#define SOURCES_SOLUTIONMESSAGE_HPP_

#include "SolutionEntry.hpp"
#include <QObject>
#include <QString>

class SolutionMessage : public SolutionEntry {
    Q_OBJECT
    Q_PROPERTY(QString text READ text CONSTANT)

public:
    explicit SolutionMessage(const QString &text, QObject *parent = nullptr)
        : SolutionEntry(parent)
        , m_text{text}
    {
    }

    QString type() const override { return "message"; };
    const QString &text() const { return m_text; }
protected:
    QString m_text;
};

#endif // SOURCES_SOLUTIONMESSAGE_HPP_
