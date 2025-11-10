//
// Created on 25 Oct, 2025
//  by alecproj
//

#ifndef SOURCES_REPORTWRITER_HPP_
#define SOURCES_REPORTWRITER_HPP_

#include "AppEnums.hpp"
#include "InputData.hpp"

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QVariant>
#include <QString>

class ReportWriter {

public:
    using Cell = std::variant<std::string, double, long long, bool>;

    ReportWriter();

    void setInputData(const InputData *inputData)
    {
        m_inputData = inputData;
    }

    int begin();

    void insertValue(const std::string &name, double value);
    void insertMessage(const std::string &text);

    /**
     * Opens a table for writing
     * @return a tableId for writeRow/endTable
     */
    int beginTable(const std::string &title, const std::vector<std::string> &columnHeaders);
    /**
     * Insertes a row to the open table with index tableId
     * @return 0 on success, -1 if the table with tableId is closed
     */
    int insertRow(int tableId, const std::vector<Cell> &row);
    /**
     * Insertes a result to the report
     */
    void insertResult(double x, double y, double funcValue);
    /**
     * Closes the table so that no more rows can be written to it
     */
    void endTable(int tableId);

    int end();

    const QString &fileName() const { return m_fileName; }

private:
    const InputData *m_inputData;
    QString m_path;
    QString m_fileName;
    QJsonObject m_report;
    QJsonArray m_solution;
    QJsonObject m_result;
    QHash<int,int> m_openTables; // tableId -> index in items
    int m_nextTableId;
    
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
     * Inserts m_solution into data.solution and m_result into
     * data.result in m_report .
     */
    inline void writeSolutionAndResult();
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
