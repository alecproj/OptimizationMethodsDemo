//
// Created on 29 Nov, 2025
//  by alecproj
//

#ifndef SOURCES_SOLUTIONENTRY_HPP_
#define SOURCES_SOLUTIONENTRY_HPP_

#include <QObject>
#include <QString>

class SolutionEntry : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString type READ type CONSTANT)

public:
    explicit SolutionEntry(QObject *parent = nullptr) : QObject(parent) {}
    virtual QString type() const = 0;
};

#endif // SOURCES_SOLUTIONENTRY_HPP_
