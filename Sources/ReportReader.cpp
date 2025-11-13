//
// Created on 4 Nov, 2025
//  by alecproj
//

#include "ReportReader.hpp"
#include "FileManager.hpp"

#include <QDateTime>
#include <QMetaEnum>
#include <QTextStream>

ReportStatus::Status ReportReader::quickInfo(const QString &fileName, QuickInfo *out)
{
    if (!out) {
        return ReportStatus::NotVerified;
    }
    FileData data;
    ReportStatus::Status rv = validateFile(fileName, &data);
    switch (rv) {
        case ReportStatus::Ok:
        case ReportStatus::InvalidCRC:
        case ReportStatus::NoSolution:
        case ReportStatus::NoResult:
            break;
        default:
            return rv;
    }
    
    out->setName(fileName);
    QString info;
    QTextStream(&info) 
        << data.abbreviation
        << data.date.toString(" dd.MM.yyyy ")
        << data.time.toString("HH:mm:ss ")
        << "f(x,y)=";
    QJsonObject dataObj = data.json.value("data").toObject();
    if (dataObj.isEmpty()) {
        return ReportStatus::InvalidDataStruct;
    }

    QJsonObject taskObj = dataObj.value("task").toObject();
    if (taskObj.isEmpty()) {
        return ReportStatus::InvalidDataStruct;
    }

    QJsonObject inputObj = taskObj.value("inputData").toObject();
    if (inputObj.isEmpty()) {
        return ReportStatus::InvalidDataStruct;
    }

    if (inputObj.contains("function") && !inputObj.value("function").isNull()) {
        QTextStream(&info) << inputObj.value("function").toString();
    } else {
        return ReportStatus::InvalidDataStruct;
    }

    out->setInfo(info);
    out->setStatus(rv);
    return ReportStatus::Ok;
}


ReportStatus::Status ReportReader::inputData(const QString &fileName, InputData *out)
{
    if (!out) {
        return ReportStatus::NotVerified;
    }
    FileData data{};
    ReportStatus::Status rv = validateFile(fileName, &data);
    switch (rv) {
        case ReportStatus::Ok:
        case ReportStatus::InvalidCRC:
        case ReportStatus::NoSolution:
        case ReportStatus::NoResult:
            break;
        default:
            return rv;
    }
    return readInputData(data.json, out);
}


ReportStatus::Status ReportReader::reportData(const QString &fileName,
    InputData *outInput, QJsonArray &outSolution, ResultData *outResult)
{
    if (!outInput) {
        return ReportStatus::NotVerified;
    }
    FileData data{};
    auto rv = validateFile(fileName, &data);
    switch (rv) {
        case ReportStatus::Ok:
        case ReportStatus::InvalidCRC:
        case ReportStatus::NoResult:
            break;
        default:
            return rv;
    }
    rv = readInputData(data.json, outInput);
    if (rv != ReportStatus::Ok) {
        return rv;
    }
    rv = readSolution(data.json, &outSolution);
    if (rv != ReportStatus::Ok) {
        return rv;
    }
    return readResult(data.json, outResult);
}

ReportStatus::Status ReportReader::readInputData(const QJsonObject &obj, InputData *out)
{
    QJsonObject dataObj = obj.value("data").toObject();
    if (dataObj.isEmpty()) {
        return ReportStatus::InvalidDataStruct;
    }

    QJsonObject taskObj = dataObj.value("task").toObject();
    if (taskObj.isEmpty()) {
        return ReportStatus::InvalidDataStruct;
    }

    QJsonObject inputObj = taskObj.value("inputData").toObject();
    if (inputObj.isEmpty()) {
        return ReportStatus::InvalidDataStruct;
    }

    if (inputObj.contains("function") && !inputObj.value("function").isNull()) {
        out->setFunction(inputObj.value("function").toString());
    } else {
        return ReportStatus::InvalidDataStruct;
    }

    if (inputObj.contains("algorithmId") && !inputObj.value("algorithmId").isNull()) {
        out->setAlgorithmId(inputObj.value("algorithmId").toInt(out->algorithmId()));
    } else {
        return ReportStatus::InvalidDataStruct;
    }

    if (inputObj.contains("extensionId") && !inputObj.value("extensionId").isNull()) {
        out->setExtensionId(inputObj.value("extensionId").toInt(out->extensionId()));
    } else {
        return ReportStatus::InvalidDataStruct;
    }

    if (inputObj.contains("fullAlgoId") && !inputObj.value("fullAlgoId").isNull()) {
        out->setFullAlgoId(inputObj.value("fullAlgoId").toInt(out->fullAlgoId()));
    } else {
        return ReportStatus::InvalidDataStruct;
    }

    if (inputObj.contains("extremumId") && !inputObj.value("extremumId").isNull()) {
        out->setExtremumId(inputObj.value("extremumId").toInt(out->extremumId()));
    } else {
        return ReportStatus::InvalidDataStruct;
    }

    if (inputObj.contains("stepId") && !inputObj.value("stepId").isNull()) {
        out->setStepId(inputObj.value("stepId").toInt(out->extremumId()));
    } else {
        return ReportStatus::InvalidDataStruct;
    }

    if (inputObj.contains("maxIterations") && !inputObj.value("maxIterations").isNull()) {
        out->setMaxIterations(inputObj.value("maxIterations").toInt(out->maxIterations()));
    } else {
        return ReportStatus::InvalidDataStruct;
    }

    if (inputObj.contains("maxFuncCalls") && !inputObj.value("maxFuncCalls").isNull()) {
        out->setMaxFuncCalls(inputObj.value("maxFuncCalls").toInt(out->maxFuncCalls()));
    } else {
        return ReportStatus::InvalidDataStruct;
    }

    if (inputObj.contains("calcAccuracy") && !inputObj.value("calcAccuracy").isNull()) {
        out->setCalcAccuracy(inputObj.value("calcAccuracy").toDouble(out->calcAccuracy()));
    } else {
        return ReportStatus::InvalidDataStruct;
    }

    if (inputObj.contains("resultAccuracy") && !inputObj.value("resultAccuracy").isNull()) {
        out->setResultAccuracy(inputObj.value("resultAccuracy").toDouble(out->resultAccuracy()));
    } else {
        return ReportStatus::InvalidDataStruct;
    }

    if (inputObj.contains("startX1") && !inputObj.value("startX1").isNull()) {
        out->setStartX1(inputObj.value("startX1").toDouble(out->startX1()));
    } else {
        return ReportStatus::InvalidDataStruct;
    }

    if (inputObj.contains("startY1") && !inputObj.value("startY1").isNull()) {
        out->setStartY1(inputObj.value("startY1").toDouble(out->startY1()));
    } else {
        return ReportStatus::InvalidDataStruct;
    }

    if (inputObj.contains("startX2") && !inputObj.value("startX2").isNull()) {
        out->setStartX2(inputObj.value("startX2").toDouble(out->startX2()));
    } else {
        return ReportStatus::InvalidDataStruct;
    }

    if (inputObj.contains("startY2") && !inputObj.value("startY2").isNull()) {
        out->setStartY2(inputObj.value("startY2").toDouble(out->startY2()));
    } else {
        return ReportStatus::InvalidDataStruct;
    }

    if (inputObj.contains("stepX") && !inputObj.value("stepX").isNull()) {
        out->setStepX(inputObj.value("stepX").toDouble(out->stepX()));
    } else {
        return ReportStatus::InvalidDataStruct;
    }

    if (inputObj.contains("stepY") && !inputObj.value("stepY").isNull()) {
        out->setStepY(inputObj.value("stepY").toDouble(out->stepY()));
    } else {
        return ReportStatus::InvalidDataStruct;
    }

    if (inputObj.contains("step") && !inputObj.value("step").isNull()) {
        out->setStep(inputObj.value("step").toDouble(out->step()));
    } else {
        return ReportStatus::InvalidDataStruct;
    }

    if (inputObj.contains("minX") && !inputObj.value("minX").isNull()) {
        out->setMinX(inputObj.value("minX").toDouble(out->minX()));
    } else {
        return ReportStatus::InvalidDataStruct;
    }

    if (inputObj.contains("maxX") && !inputObj.value("maxX").isNull()) {
        out->setMaxX(inputObj.value("maxX").toDouble(out->maxX()));
    } else {
        return ReportStatus::InvalidDataStruct;
    }

    if (inputObj.contains("minY") && !inputObj.value("minY").isNull()) {
        out->setMinY(inputObj.value("minY").toDouble(out->minY()));
    } else {
        return ReportStatus::InvalidDataStruct;
    }

    if (inputObj.contains("maxY") && !inputObj.value("maxY").isNull()) {
        out->setMaxY(inputObj.value("maxY").toDouble(out->maxY()));
    } else {
        return ReportStatus::InvalidDataStruct;
    }

    return ReportStatus::Ok;
}

ReportStatus::Status ReportReader::readSolution(const QJsonObject &obj, QJsonArray *out)
{
    QJsonObject dataObj = obj.value("data").toObject();
    if (dataObj.isEmpty()) {
        return ReportStatus::InvalidDataStruct;
    }
    *out = dataObj.value("solution").toArray();
    if (out->isEmpty()) {
        return ReportStatus::NoSolution;
    }
    return ReportStatus::Ok;
}

ReportStatus::Status ReportReader::readResult(const QJsonObject &obj, ResultData *out)
{
    QJsonObject dataObj = obj.value("data").toObject();
    if (dataObj.isEmpty()) {
        return ReportStatus::InvalidDataStruct;
    }

    QJsonObject resultObj = dataObj.value("result").toObject();
    if (resultObj.isEmpty()) {
        return ReportStatus::NoResult;
    }

    if (resultObj.contains("x") && !resultObj.value("x").isNull()) {
        out->setXValue(resultObj.value("x").toDouble(out->xValue()));
    } else {
        return ReportStatus::InvalidDataStruct;
    }

    if (resultObj.contains("y") && !resultObj.value("y").isNull()) {
        out->setYValue(resultObj.value("y").toDouble(out->yValue()));
    } else {
        return ReportStatus::InvalidDataStruct;
    }

    if (resultObj.contains("funcValue") && !resultObj.value("funcValue").isNull()) {
        out->setFuncValue(resultObj.value("funcValue").toDouble(out->funcValue()));
    } else {
        return ReportStatus::InvalidDataStruct;
    }

    return ReportStatus::Ok;
}

ReportStatus::Status ReportReader::validateFile(const QString &fileName, FileData *out)
{
    if (!FileManager::fileExists(fileName)) {
        return ReportStatus::FileDoesNotExists;
    }
    if (!validateName(fileName, out)) {
        return ReportStatus::InvalidName;
    }
    if (!FileManager::loadJsonFile(fileName, out->json)) {
        return ReportStatus::InvalidFile;
    }
    auto dataObj = out->json.value("data").toObject();
    if (dataObj.isEmpty()) {
        return ReportStatus::InvalidDataStruct;
    }
    auto solutionArr = dataObj.value("solution").toArray();
    if (solutionArr.isEmpty()) {
        return ReportStatus::NoSolution;
    }
    auto resultObj = dataObj.value("result").toObject();
    if (resultObj.isEmpty()) {
        return ReportStatus::NoResult;
    }
    ReportStatus::Status rv = validateCRC(out->json);
    return rv;
}

bool ReportReader::validateName(const QString &fileName, FileData *out)
{
    static const QRegularExpression re(QStringLiteral(
        "^([BCDGSR]+)-(\\d{1,2})-(\\d{1,2})-(\\d{4})-(\\d{1,2})-(\\d{1,2})-(\\d{1,2})\\.json$"
    ), QRegularExpression::CaseInsensitiveOption);

    QRegularExpressionMatch match;
    match = re.matchView(QStringView(fileName));
    if (!match.hasMatch()) return false;

    const QString enumStr = match.captured(1);
    const int day    = match.captured(2).toInt();
    const int month  = match.captured(3).toInt();
    const int year   = match.captured(4).toInt();
    const int hour   = match.captured(5).toInt();
    const int minute = match.captured(6).toInt();
    const int second = match.captured(7).toInt();

    QMetaEnum me = QMetaEnum::fromType<FullAlgoType::Type>();
    int enumVal = me.keyToValue(enumStr.toLatin1().constData());
    if (enumVal == -1) return false;

    if (!QDate::isValid(year, month, day)) return false;
    if (!QTime::isValid(hour, minute, second)) return false;

    if (out != nullptr) {
        out->abbreviation = enumStr;
        out->fullType = static_cast<FullAlgoType::Type>(enumVal);
        out->date = QDate(year, month, day);
        out->time = QTime(hour, minute, second);
    }
    return true;
}


ReportStatus::Status ReportReader::validateCRC(const QJsonObject &obj)
{
    QJsonValue value = obj.value("checksum");
    if (value.isNull()) {
        return ReportStatus::CheckFailed;
    }

    uint32_t crc = calculateCRC(obj);
    uint32_t parsed = 0;

    if (value.isString()) {
        QString strValue = value.toString().trimmed();
        if (strValue.startsWith("0X", Qt::CaseInsensitive))
            strValue = strValue.mid(2);
        bool rv = false;
        parsed = strValue.toUInt(&rv, 16);
        if (!rv) {
            return ReportStatus::CheckFailed;
        }
    } else if (value.isDouble()) {
        double num = value.toDouble();
        parsed = static_cast<uint32_t>(static_cast<uint64_t>(qRound64(num)));
    } else {
        return ReportStatus::InvalidDataStruct;
    }

    if (parsed == crc) {
        return ReportStatus::Ok;
    } else {
        return ReportStatus::InvalidCRC;
    }
}

uint32_t ReportReader::calculateCRC(const QJsonObject &obj)
{
    QJsonObject dataObj = obj.value("data").toObject();
    QJsonDocument doc(dataObj);
    QByteArray ba = doc.toJson(QJsonDocument::Compact);
    return FileManager::crc32FromBytes(ba);
}
