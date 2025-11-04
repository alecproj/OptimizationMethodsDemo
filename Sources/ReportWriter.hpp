//
// Created on 25 Oct, 2025
//  by alecproj
//

#ifndef SOURCES_REPORTWRITER_HPP_
#define SOURCES_REPORTWRITER_HPP_

#include "AppEnums.hpp"
#include "InputData.hpp"

#include <QJsonObject>

class ReportWriter {

public:
    explicit ReportWriter();

    void setInputData(const InputData *inputData)
    {
        m_inputData = inputData;
    }

    int begin();
    int end();

private:
    const InputData *m_inputData;
    QString m_path;
    QString m_fileName;
    QJsonObject m_report;
    
    /**
     * Creates the initial structure in m_report.
     * { "checksum": null, "data": { "task": {}, "solution": {}, "result": {} } }
     */
    inline void prepare();
    /**
     * Inserts m_inputData into data.task.inputData in m_report.
     */
    inline void writeInputData();
    /**
     * Inserts CRC calculated with calculateCRC method into m_report.
     */
    inline void writeCRC();

    inline QString fileName(FullAlgoType::Type type);
        /**
     * Calculates a checksum from the entire contents of the data section
     * in m_report.
     */
    inline uint32_t calculateCRC();
};

#endif // SOURCES_REPORTWRITER_HPP_
