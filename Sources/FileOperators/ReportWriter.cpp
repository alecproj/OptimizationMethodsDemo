//
// Created on 25 Oct, 2025
//  by alecproj
//

#include "ReportWriter.hpp"
#include "FileManager.hpp"
#include "LOInputData.hpp"
#include "GOInputData.hpp"

#include <QString>
#include <QDateTime>
#include <QMetaEnum>
#include <QDebug>

// Helper: convert std::string -> QString
static inline QString qs(const std::string &s) { return QString::fromStdString(s); }

// Convert Cell variant to QJsonValue
static QJsonValue cellToJsonValue(const ReportWriter::Cell &c) {
    if (std::holds_alternative<std::string>(c)) {
        return QJsonValue(QString::fromStdString(std::get<std::string>(c)));
    } else if (std::holds_alternative<double>(c)) {
        return QJsonValue(std::get<double>(c));
    } else if (std::holds_alternative<long long>(c)) {
        return QJsonValue(static_cast<qint64>(std::get<long long>(c)));
    } else if (std::holds_alternative<bool>(c)) {
        return QJsonValue(std::get<bool>(c));
    } else {
        return QJsonValue(); // null
    }
}

ReportWriter::ReportWriter()
    : m_inputData{nullptr}
    , m_path{QString{}}
    , m_fileName{QString{}}
    , m_report{QJsonObject{}}
    , m_solution{}
    , m_result{}
    , m_openTables{}
    , m_nextTableId{1}
{
}

int ReportWriter::begin()
{
    if (!m_inputData) {
        return -1;
    }
    if (!FileManager::ensureBaseDirExists(m_path)) {
        return -2;
    }
    auto partType = m_inputData->partitionId();

    if (partType == PartType::LO) {
        qDebug() << "Begin writing a report with LO InputData";
        auto data = dynamic_cast<const LO::InputData *>(m_inputData);
        m_fileName = fileNameLO(
            static_cast<LO::FullAlgoType::Type>(data->fullAlgoId())
        );
    } else if (partType == PartType::GO) {
        qDebug() << "Begin writing a report with GO InputData";
        auto data = dynamic_cast<const GO::InputData *>(m_inputData);
        m_fileName = fileNameGO(
            static_cast<AlgoType::Type>(data->algorithmId())
        );
    } else {
        qCritical() << "Unable to begin report. Invalid part type";
        return -3;
    }
    qDebug() << "FileName: " << m_fileName;
    m_solution = QJsonArray{};
    m_openTables.clear();
    m_nextTableId = 1;
    prepare();
    writeInputData();
    return 0;
}

void ReportWriter::insertValue(const std::string &name, double value)
{
    QJsonObject obj;
    obj["type"] = "value";
    obj["title"] = qs(name);
    obj["value"] = value;
    m_solution.append(obj);
}

void ReportWriter::insertMessage(const std::string &text)
{
    QJsonObject obj;
    obj["type"] = "message";
    obj["text"] = qs(text);
    m_solution.append(obj);
}

int ReportWriter::beginTable(const std::string &title, const std::vector<std::string> &columnHeaders)
{
    QJsonObject tbl;
    tbl["type"] = "table";
    tbl["title"] = qs(title);
    QJsonArray cols;
    for (const auto &c : columnHeaders) cols.append(qs(c));
    tbl["columns"] = cols;
    tbl["rows"] = QJsonArray();
    m_solution.append(tbl);
    int index = m_solution.size() - 1;

    int tableId = m_nextTableId++;
    m_openTables.insert(tableId, index);
    return tableId;
}

int ReportWriter::insertRow(int tableId, const std::vector<Cell> &row)
{
    if (!m_openTables.contains(tableId)) {
        qWarning("ReportWriter::writeRow: tableId not open");
        return -1;
    }
    int idx = m_openTables.value(tableId);
    QJsonObject tbl = m_solution.at(idx).toObject();
    QJsonArray rows = tbl["rows"].toArray();

    QJsonArray jsonRow;
    for (const auto &cell : row) {
        jsonRow.append(cellToJsonValue(cell));
    }
    rows.append(jsonRow);
    tbl["rows"] = rows;
    m_solution.replace(idx, tbl);
    return 0;
}

void ReportWriter::endTable(int tableId)
{
    m_openTables.remove(tableId);
}

void ReportWriter::insertResult(double x, double y, double funcValue)
{
    m_result["x"] = x;
    m_result["y"] = y;
    m_result["funcValue"] = funcValue;
}

int ReportWriter::end()
{
    writeSolutionAndResult();
    writeCRC();
    if (!FileManager::saveJsonFile(m_fileName, m_report)) {
        return -1;
    }
    return 0;
}

void ReportWriter::prepare()
{
    QJsonObject root;
    root.insert("checksum", QJsonValue(QJsonValue::Null));

    QJsonObject data;
    data.insert("task", QJsonObject());
    data.insert("solution", QJsonObject());
    data.insert("result", QJsonObject());

    root.insert("data", data);
    m_report = root;
}

void ReportWriter::writeInputData()
{
    QJsonObject inputData;
    auto partType = m_inputData->partitionId();
    if (partType == PartType::LO) {
        auto idata = static_cast<const LO::InputData *>(m_inputData);
        inputData.insert("partitionId", idata->partitionId());
        inputData.insert("function", idata->function());
        inputData.insert("algorithmId", idata->algorithmId());
        inputData.insert("extensionId", idata->extensionId());
        inputData.insert("fullAlgoId", idata->fullAlgoId());
        inputData.insert("extremumId", idata->extremumId());
        inputData.insert("stepId", idata->stepId());
        inputData.insert("maxIterations", idata->maxIterations());
        inputData.insert("maxFuncCalls", idata->maxFuncCalls());
        inputData.insert("calcAccuracy", idata->calcAccuracy());
        inputData.insert("resultAccuracy", idata->resultAccuracy());
        inputData.insert("startX1", idata->startX1());
        inputData.insert("startY1", idata->startY1());
        inputData.insert("startX2", idata->startX2());
        inputData.insert("startY2", idata->startY2());
        inputData.insert("stepX", idata->stepX());
        inputData.insert("stepY", idata->stepY());
        inputData.insert("step", idata->step());
        inputData.insert("minX", idata->minX());
        inputData.insert("maxX", idata->maxX());
        inputData.insert("minY", idata->minY());
        inputData.insert("maxY", idata->maxY());
        qDebug() << "Writing LO Input Data";
    } else if (partType == PartType::GO) {
        auto idata = static_cast<const GO::InputData *>(m_inputData);
        inputData.insert("partitionId", idata->partitionId());
        inputData.insert("function", idata->function());
        inputData.insert("algorithmId", idata->algorithmId());
        inputData.insert("extremumId", idata->extremumId());
        inputData.insert("calcAccuracy", idata->calcAccuracy());
        inputData.insert("resultAccuracy", idata->resultAccuracy());
        inputData.insert("minX", idata->minX());
        inputData.insert("maxX", idata->maxX());
        inputData.insert("minY", idata->minY());
        inputData.insert("maxY", idata->maxY());
        inputData.insert("size", idata->size());
        inputData.insert("maxIterations", idata->maxIterations());
        inputData.insert("crossoverProb", idata->crossoverProb());
        inputData.insert("mutationProb", idata->mutationProb());
        inputData.insert("elitism", idata->elitism());
        inputData.insert("inertiaCoef", idata->inertiaCoef());
        inputData.insert("cognitiveCoef", idata->cognitiveCoef());
        inputData.insert("socialCoef", idata->socialCoef());
        qDebug() << "Writing GO Input Data";
    } else {
        qCritical() << "Unable to write Input Data. Invalid type.";
    }

    QJsonObject dataObj = m_report.value("data").toObject();
    QJsonObject taskObj = dataObj.value("task").toObject();
    taskObj.insert("inputData", inputData);
    dataObj.insert("task", taskObj);
    m_report.insert("data", dataObj);
}

void ReportWriter::writeSolutionAndResult()
{
    qDebug() << "Writing solution and result...";
    QJsonObject dataObj = m_report.value("data").toObject();
    dataObj.insert("solution", m_solution);
    dataObj.insert("result", m_result);
    m_report.insert("data", dataObj);
}

void ReportWriter::writeCRC()
{
    qDebug() << "Writing checksum...";
    uint32_t crc = calculateCRC();
    QString hex = QString("0x%1").arg(crc, 8, 16, QLatin1Char('0')).toUpper();
    m_report.insert("checksum", hex);
}

QString ReportWriter::fileNameLO(LO::FullAlgoType::Type type)
{
    QMetaEnum meta = QMetaEnum::fromType<LO::FullAlgoType::Type>();
    const char *key = meta.valueToKey(static_cast<int>(type));
    QString enumName = key ? QString::fromLatin1(key) : QString::number(static_cast<int>(type));

    QString timestamp = QDateTime::currentDateTime().toString("dd-MM-yyyy-HH-mm-ss");
    return QString("LO-%1-%2.json").arg(enumName, timestamp);
}

QString ReportWriter::fileNameGO(AlgoType::Type type)
{
    QMetaEnum meta = QMetaEnum::fromType<AlgoType::Type>();
    const char *key = meta.valueToKey(static_cast<int>(type));
    QString enumName = key ? QString::fromLatin1(key) : QString::number(static_cast<int>(type));

    QString timestamp = QDateTime::currentDateTime().toString("dd-MM-yyyy-HH-mm-ss");
    return QString("GO-%1-%2.json").arg(enumName, timestamp);
}

uint32_t ReportWriter::calculateCRC()
{
    QJsonObject dataObj = m_report.value("data").toObject();
    QJsonDocument doc(dataObj);
    QByteArray ba = doc.toJson(QJsonDocument::Compact);
    return FileManager::crc32FromBytes(ba);
}
