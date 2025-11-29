//
// Created on 4 Nov, 2025
//  by alecproj
//

#ifndef SOURCES_REPORTREADER_HPP_
#define SOURCES_REPORTREADER_HPP_

#include "InputData.hpp"
#include "LOInputData.hpp"
#include "GOInputData.hpp"
#include "QuickInfo.hpp"
#include "ResultData.hpp"
#include "AppEnums.hpp"

#include <QString>
#include <QDateTime>
#include <QJsonObject>
#include <QJsonArray>

struct FileData {
    QJsonObject json;
    QString abbreviation;
    PartType::Type partType;

    union {
        LO::FullAlgoType::Type full;
        AlgoType::Type base;
    } algoType;

    QDate date;
    QTime time;
};

class ReportReader {

public:

    static ReportStatus::Status quickInfo(const QString &fileName,
        QuickInfo *out, PartType::Type type = PartType::NONE);
    static ReportStatus::Status inputData(const QString &fileName,
        InputData *out);
    static ReportStatus::Status reportData(const QString &fileName,
        InputData *&outInput, QJsonArray &outSolution, ResultData *outResult);

private:
    static inline ReportStatus::Status readInputData(const QJsonObject &obj, LO::InputData *out);
    static inline ReportStatus::Status readInputData(const QJsonObject &obj, GO::InputData *out);
    static inline ReportStatus::Status readSolution(const QJsonObject &obj, QJsonArray *out);
    static inline ReportStatus::Status readResult(const QJsonObject &obj, ResultData *out);

    static inline ReportStatus::Status validateFile(const QString &fileName, FileData *out);
    static inline bool validateName(const QString &fileName, FileData *out = nullptr);
    static inline ReportStatus::Status validateCRC(const QJsonObject &obj);
    /**
     * Calculates a checksum from the entire contents of the data section
     * in obj.
     * @return checksum
     */
    static inline uint32_t calculateCRC(const QJsonObject &obj);
};


#endif // SOURCES_REPORTREADER_HPP_
