//
// Created on 3 Nov, 2025
//  by alecproj
//

#ifndef SOURCES_MAINCONTROLLER_HPP_
#define SOURCES_MAINCONTROLLER_HPP_

#include "AppEnums.hpp"
#include "ReportWriter.hpp"
#include "QuickInfoListModel.hpp"

#include <CoordinateDescent/CoordinateDescent.hpp>
#include <GradientDescent/GradientDescent.hpp>

#include <QObject>

using Status = Result::Type;
using GDAlgoType = GradientDescent<ReportWriter>;
using CDAlgoType = CoordinateDescent<ReportWriter>;

class MainController : public QObject {
    Q_OBJECT

    Q_PROPERTY(QuickInfoListModel *quickInfoModel READ quickInfoModel NOTIFY quickInfoModelChanged)
public:

    explicit MainController(QObject *parent = nullptr);

    Q_INVOKABLE Status setInputData(const InputData *data);
    Q_INVOKABLE Status solve();
    Q_INVOKABLE void updateQuickInfoModel();
    Q_INVOKABLE Status inputDataFromFile(const QString &fileName, InputData *out);

    QuickInfoListModel *quickInfoModel() { return &m_quickInfoModel; }

signals:
    void quickInfoModelChanged();

private:
    ReportWriter m_writer;
    AlgoType::Type m_currAlgorithm;
    ExtensionType::Type m_currExtension;
    CDAlgoType m_cdAlgo;
    CoordinateInput m_cdData;
    GDAlgoType m_gdAlgo;
    GradientInput m_gdData;
    QuickInfoListModel m_quickInfoModel;

};

#endif // SOURCES_MAINCONTROLLER_HPP_
