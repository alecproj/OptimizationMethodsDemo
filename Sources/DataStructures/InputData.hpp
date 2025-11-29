//
// Created on 3 Nov, 2025
//  by alecproj
//

#ifndef SOURCES_INPUTDATA_HPP_
#define SOURCES_INPUTDATA_HPP_

#include "AppEnums.hpp"
#include <QObject>
#include <QString>

class InputData : public QObject {
    Q_OBJECT
    Q_PROPERTY(int partitionId READ partitionId CONSTANT)
    Q_PROPERTY(int algorithmId READ algorithmId WRITE setAlgorithmId NOTIFY algorithmIdChanged)

public:
    explicit InputData(QObject *parent = nullptr) : QObject(parent) {}
    virtual PartType::Type partitionId() const = 0;
    virtual AlgoType::Type algorithmId() const = 0;

public slots:
    virtual void setAlgorithmId(int v) = 0;

signals:
    void algorithmIdChanged();
};

#endif // SOURCES_INPUTDATA_HPP_
