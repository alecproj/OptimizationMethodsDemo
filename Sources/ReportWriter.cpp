//
// Created on 25 Oct, 2025
//  by alecproj
//

#include "ReportWriter.hpp"
#include "FileManager.hpp"

#include <QString>
#include <QDateTime>
#include <QMetaEnum>

ReportWriter::ReportWriter()
    : m_inputData{nullptr}
    , m_path{QString{}}
    , m_fileName{QString{}}
    , m_report{QJsonObject{}}
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
    m_fileName = fileName(
        static_cast<FullAlgoType::Type>(m_inputData->fullAlgoId())
    );
    prepare();
    writeInputData();
    return 0;
}

int ReportWriter::end()
{
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
    inputData.insert("function", m_inputData->function());
    inputData.insert("algorithmId", m_inputData->algorithmId());
    inputData.insert("extensionId", m_inputData->extensionId());
    inputData.insert("fullAlgoId", m_inputData->fullAlgoId());
    inputData.insert("extremumId", m_inputData->extremumId());
    inputData.insert("maxIterations", m_inputData->maxIterations());
    inputData.insert("calcAccuracy", m_inputData->calcAccuracy());
    inputData.insert("resultAccuracy", m_inputData->resultAccuracy());
    inputData.insert("startX1", m_inputData->startX1());
    inputData.insert("startY1", m_inputData->startY1());
    inputData.insert("startX2", m_inputData->startX2());
    inputData.insert("startY2", m_inputData->startY2());
    inputData.insert("stepX", m_inputData->stepX());
    inputData.insert("stepY", m_inputData->stepY());
    inputData.insert("coefficientStep", m_inputData->coefficientStep());
    inputData.insert("minX", m_inputData->minX());
    inputData.insert("maxX", m_inputData->maxX());
    inputData.insert("minY", m_inputData->minY());
    inputData.insert("maxY", m_inputData->maxY());

    QJsonObject dataObj = m_report.value("data").toObject();
    QJsonObject taskObj = dataObj.value("task").toObject();
    taskObj.insert("inputData", inputData);
    dataObj.insert("task", taskObj);
    m_report.insert("data", dataObj);
}

void ReportWriter::writeCRC()
{
    uint32_t crc = calculateCRC();
    QString hex = QString("0x%1").arg(crc, 8, 16, QLatin1Char('0')).toUpper();
    m_report.insert("checksum", hex);
}

QString ReportWriter::fileName(FullAlgoType::Type type)
{
    QMetaEnum meta = QMetaEnum::fromType<FullAlgoType::Type>();
    const char *key = meta.valueToKey(static_cast<int>(type));
    QString enumName = key ? QString::fromLatin1(key) : QString::number(static_cast<int>(type));

    QString timestamp = QDateTime::currentDateTime().toString("dd-MM-yyyy-HH-mm-ss");
    return QString("%1-%2.json").arg(enumName, timestamp);
}

uint32_t ReportWriter::calculateCRC()
{
    QJsonObject dataObj = m_report.value("data").toObject();
    QJsonDocument doc(dataObj);
    QByteArray ba = doc.toJson(QJsonDocument::Compact);
    return FileManager::crc32FromBytes(ba);
}
