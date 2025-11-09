//
// Created on 6 Nov, 2025
//  by alecproj
//

#ifndef SOURCES_REPORT_HPP_
#define SOURCES_REPORT_HPP_

#include "InputData.hpp"
#include "SolutionModel.hpp"
#include "ResultData.hpp"

#include <QObject>
#include <QString>

class Report : public QObject {
    Q_OBJECT

    Q_PROPERTY(QString fileName READ fileName CONSTANT)
    Q_PROPERTY(InputData *inputData READ inputData CONSTANT)
    Q_PROPERTY(SolutionModel *solution READ solution CONSTANT)
    Q_PROPERTY(ResultData *resultData READ resultData CONSTANT)
public:
    QString m_fileName;

    Report(
        const QString &fileName,
        InputData *inputData,
        SolutionModel *solution,
        ResultData *resultData,
        QObject *parent = nullptr
    )
        : QObject{parent}
        , m_fileName{fileName}
        , m_inputData{inputData}
        , m_solution{solution}
        , m_resultData{resultData}
    {
    }

    QString &fileName() { return m_fileName; }
    InputData *inputData() { return m_inputData; }
    SolutionModel *solution() { return m_solution; }
    ResultData *resultData() { return m_resultData; }

private:
    InputData *m_inputData;
    SolutionModel *m_solution;
    ResultData *m_resultData;

};

#endif // SOURCES_REPORT_HPP_
