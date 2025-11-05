//
// Created on 4 Nov, 2025
//  by alecproj
//

#ifndef SOURCES_REPORTREADER_HPP_
#define SOURCES_REPORTREADER_HPP_

#include "InputData.hpp"
#include "QuickInfo.hpp"
#include "AppEnums.hpp"

#include <QString>
#include <QDateTime>
#include <QJsonObject>

struct FileData {
    QJsonObject json;
    QString abbreviation;
    FullAlgoType::Type fullType;
    QDate date;
    QTime time;
};

class ReportReader {

public:

    static ReportStatus::Status quickInfo(const QString &fileName, QuickInfo *out);
    static ReportStatus::Status inputData(const QString &fileName, InputData *out);

private:
    static inline ReportStatus::Status readInputData(const QJsonObject &obj, InputData *out);

    static inline ReportStatus::Status validateFile(const QString &fileName, FileData *out = nullptr);
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
