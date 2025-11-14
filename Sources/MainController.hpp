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
#include <ConjugateGradient/ConjugateGradient.hpp>

#include <QObject>

using Status = Result::Type;
using CDAlgoType = CD::CoordinateDescent<ReportWriter>;
using GDAlgoType = GD::GradientDescent<ReportWriter>;
using CGAlgoType = CG::ConjugateGradient<ReportWriter>;

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
    Q_INVOKABLE Status openReport(const QString &fileName);
    Q_INVOKABLE void closeReport(const QString &fileName);
    Q_INVOKABLE void requestDeleteReport(const QString &fileName);
    Q_INVOKABLE int openReportsCount() { return m_openReports.count(); }
    QList<Report *> &openReports() { return m_openReports; }
    Q_INVOKABLE void askConfirm(const QString &title, const QString &text, bool twoButtons = false) {
        emit requestConfirm(title, text, twoButtons);
    }

    QuickInfoListModel *quickInfoModel() { return &m_quickInfoModel; }

signals:
    void quickInfoModelChanged();
    void openReportsUpdated();
    void requestConfirm(const QString &title, const QString &text, bool twoButtons);

public slots:
    void deleteConfirmed();

private:
    ReportWriter m_writer;
    AlgoType::Type m_currAlgorithm;
    ExtensionType::Type m_currExtension;
    CDAlgoType m_cdAlgo;
    CD::InputData m_cdData;
    GDAlgoType m_gdAlgo;
    GD::InputData m_gdData;
    QuickInfoListModel m_quickInfoModel;
    CGAlgoType m_cgAlgo;
    CG::InputData m_cgData;
    QList<Report *> m_openReports;
    QString m_filePendingDeletion;
    EnumHelper m_enumHelper;

    void fillCDData(const InputData *data);
    void fillGDData(const InputData *data);
    void fillCGData(const InputData *data);
};

#endif // SOURCES_MAINCONTROLLER_HPP_
