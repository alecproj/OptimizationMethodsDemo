//
// Created on 3 Nov, 2025
//  by alecproj
//

#ifndef SOURCES_MAINCONTROLLER_HPP_
#define SOURCES_MAINCONTROLLER_HPP_

#include "AppEnums.hpp"
#include "ReportWriter.hpp"
#include "Report.hpp"
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
    Q_PROPERTY(QList<Report *> openReports READ openReports NOTIFY openReportsUpdated)
    Q_PROPERTY(int openReportsCount READ openReportsCount NOTIFY openReportsUpdated)
public:

    explicit MainController(QObject *parent = nullptr);

    Q_INVOKABLE Status setInputData(const InputData *data);
    Q_INVOKABLE Status solve();
    Q_INVOKABLE void updateQuickInfoModel();
    Q_INVOKABLE Status inputDataFromFile(const QString &fileName, InputData *out);
    Q_INVOKABLE void openReport(const QString &fileName);
    Q_INVOKABLE int openReportsCount() { return m_openReports.count(); }
    QList<Report *> &openReports() { return m_openReports; }

    QuickInfoListModel *quickInfoModel() { return &m_quickInfoModel; }

signals:
    void quickInfoModelChanged();
    void openReportsUpdated();

private:
    ReportWriter m_writer;
    AlgoType::Type m_currAlgorithm;
    ExtensionType::Type m_currExtension;
    CDAlgoType m_cdAlgo;
    CoordinateInput m_cdData;
    GDAlgoType m_gdAlgo;
    GradientInput m_gdData;
    QuickInfoListModel m_quickInfoModel;
    QList<Report *> m_openReports;

};

#endif // SOURCES_MAINCONTROLLER_HPP_
