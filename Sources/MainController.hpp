//
// Created on 3 Nov, 2025
//  by alecproj
//

#ifndef SOURCES_MAINCONTROLLER_HPP_
#define SOURCES_MAINCONTROLLER_HPP_

#include "AppEnums.hpp"
#include "ReportWriter.hpp"

#include <CoordinateDescent/CoordinateDescent.hpp>
#include <GradientDescent/GradientDescent.hpp>

#include <QObject>

using Status = Result::Type;
using GDAlgoType = GradientDescent<ReportWriter>;
using CDAlgoType = CoordinateDescent<ReportWriter>;

class MainController : public QObject {
    Q_OBJECT
public:

    explicit MainController(QObject *parent = nullptr);

    Q_INVOKABLE Status setInputData(const InputData *data);
    Q_INVOKABLE Status solve();

private:
    ReportWriter m_writer;
    AlgoType::Type m_currAlgorithm;
    ExtensionType::Type m_currExtension;
    CDAlgoType m_cdAlgo;
    CoordinateInput m_cdData;
    GDAlgoType m_gdAlgo;
    GradientInput m_gdData;

};

#endif // SOURCES_MAINCONTROLLER_HPP_
