//
// Created on 4 Nov, 2025
//  by alecproj
//

#include "ReportReader.hpp"
#include "FileManager.hpp"

#include <QtLogging>
#include <QDateTime>
#include <QMetaEnum>
#include <QTextStream>

#define READ_FIELD(obj, key, out, setter, getter, convMethod)                \
    do {                                                                     \
        if ((obj).contains(key) && !(obj).value(key).isNull()) {             \
            (out)->setter((obj).value(key).convMethod((out)->getter()));     \
        } else {                                                             \
            qWarning() << "Cannot read" << key;                              \
            return ReportStatus::InvalidDataStruct;                          \
        }                                                                    \
    } while (0)

#define READ_OBJ(parentObj, key, outObj)                                     \
    QJsonObject outObj = (parentObj).value(key).toObject();                  \
    if (outObj.isEmpty()) {                                                  \
        qWarning() << "The" key "object is empty";                           \
        return ReportStatus::InvalidDataStruct;                              \
    }

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
    qDebug() << "Reading the inputData from" << fileName << "...";
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
    if (data.partType == PartType::LO) {
        auto idata = dynamic_cast<LO::InputData *>(out);
        return readInputData(data.json, idata);
    } else if (data.partType == PartType::GO) {
        auto idata = dynamic_cast<GO::InputData *>(out);
        return readInputData(data.json, idata);
    } else {
        qCritical() << "Invalid PartType in InputData";
        return ReportStatus::NotVerified;
    }
}


ReportStatus::Status ReportReader::reportData(const QString &fileName,
    InputData *&outInput, QJsonArray &outSolution, ResultData *outResult)
{
    qDebug() << "Reading the report" << fileName << "...";
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

    if (data.partType == PartType::LO) {
        outInput = new LO::InputData();
        rv = readInputData(data.json,
            static_cast<LO::InputData *>(outInput));
    } else if (data.partType == PartType::GO) {
        outInput = new GO::InputData();
        rv = readInputData(data.json,
            static_cast<GO::InputData *>(outInput));
    } else {
        qCritical() << "Invalid PartType in FileData";
        return ReportStatus::NotVerified;
    }
    if (rv != ReportStatus::Ok) {
        return rv;
    }
    rv = readSolution(data.json, &outSolution);
    if (rv != ReportStatus::Ok) {
        return rv;
    }
    return readResult(data.json, outResult);
}

ReportStatus::Status ReportReader::readInputData(const QJsonObject &obj, LO::InputData *out)
{
    READ_OBJ(obj, "data", dataObj);
    READ_OBJ(dataObj, "task", taskObj);
    READ_OBJ(taskObj, "inputData", inputObj);

    READ_FIELD(inputObj, "function", out,
            setFunction, function, toString);
    READ_FIELD(inputObj, "algorithmId", out,
            setAlgorithmId, algorithmId, toInt);
    READ_FIELD(inputObj, "extensionId", out,
            setExtensionId, extensionId, toInt);
    READ_FIELD(inputObj, "fullAlgoId", out,
            setFullAlgoId, fullAlgoId, toInt);
    READ_FIELD(inputObj, "extremumId", out,
            setExtremumId, extremumId, toInt);
    READ_FIELD(inputObj, "stepId", out,
            setStepId, stepId, toInt);
    READ_FIELD(inputObj, "maxIterations", out,
            setMaxIterations, maxIterations, toInt);
    READ_FIELD(inputObj, "maxFuncCalls", out,
            setMaxFuncCalls, maxFuncCalls, toInt);
    READ_FIELD(inputObj, "resultAccuracy", out,
            setResultAccuracy, resultAccuracy, toInt);
    READ_FIELD(inputObj, "calcAccuracy", out,
            setCalcAccuracy, calcAccuracy, toInt);
    READ_FIELD(inputObj, "startX1", out,
            setStartX1, startX1, toDouble);
    READ_FIELD(inputObj, "startY1", out,
            setStartY1, startY1, toDouble);
    READ_FIELD(inputObj, "startX2", out,
            setStartX2, startX2, toDouble);
    READ_FIELD(inputObj, "startY2", out,
            setStartY2, startY2, toDouble);
    READ_FIELD(inputObj, "stepX", out,
            setStepX, stepX, toDouble);
    READ_FIELD(inputObj, "stepY", out,
            setStepY, stepY, toDouble);
    READ_FIELD(inputObj, "step", out,
            setStep, step, toDouble);
    READ_FIELD(inputObj, "minX", out,
            setMinX, minX, toDouble);
    READ_FIELD(inputObj, "maxX", out,
            setMaxX, maxX, toDouble);
    READ_FIELD(inputObj, "minY", out,
            setMinY, minY, toDouble);
    READ_FIELD(inputObj, "maxY", out,
            setMaxY, maxY, toDouble);
    return ReportStatus::Ok;
}

ReportStatus::Status ReportReader::readInputData(const QJsonObject &obj, GO::InputData *out)
{
    READ_OBJ(obj, "data", dataObj);
    READ_OBJ(dataObj, "task", taskObj);
    READ_OBJ(taskObj, "inputData", inputObj);

    READ_FIELD(inputObj, "function", out,
            setFunction, function, toString);
    READ_FIELD(inputObj, "algorithmId", out,
            setAlgorithmId, algorithmId, toInt);
    READ_FIELD(inputObj, "extremumId", out,
            setExtremumId, extremumId, toInt);
    READ_FIELD(inputObj, "resultAccuracy", out,
            setResultAccuracy, resultAccuracy, toInt);
    READ_FIELD(inputObj, "minX", out,
            setMinX, minX, toDouble);
    READ_FIELD(inputObj, "maxX", out,
            setMaxX, maxX, toDouble);
    READ_FIELD(inputObj, "minY", out,
            setMinY, minY, toDouble);
    READ_FIELD(inputObj, "maxY", out,
            setMaxY, maxY, toDouble);

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
    qDebug() << "File validation" << fileName << "...";
    if (!FileManager::fileExists(fileName)) {
        return ReportStatus::FileDoesNotExists;
    }
    if (!validateName(fileName, out)) {
        qWarning() << "Name validation failed";
        return ReportStatus::InvalidName;
    }
    if (!FileManager::loadJsonFile(fileName, out->json)) {
        qWarning() << "Unable to load json";
        return ReportStatus::InvalidFile;
    }
    auto dataObj = out->json.value("data").toObject();
    if (dataObj.isEmpty()) {
        qWarning() << "The data section is empty";
        return ReportStatus::InvalidDataStruct;
    }
    auto solutionArr = dataObj.value("solution").toArray();
    if (solutionArr.isEmpty()) {
        qWarning() << "The solution section is empty";
        return ReportStatus::NoSolution;
    }
    auto resultObj = dataObj.value("result").toObject();
    if (resultObj.isEmpty()) {
        qWarning() << "The result section is empty";
        return ReportStatus::NoResult;
    }
    auto rv = validateCRC(out->json);
    if (rv != ReportStatus::Ok) {
        qWarning() << "Checksum validation failed";
    }
    return rv;
}

bool ReportReader::validateName(const QString &fileName, FileData *out)
{
    static const QRegularExpression re(QStringLiteral(
        "^(?:(LO|GO)-)?([BCDGSR]+)-(\\d{1,2})-(\\d{1,2})-(\\d{4})-(\\d{1,2})-(\\d{1,2})-(\\d{1,2})\\.json$"
    ), QRegularExpression::CaseInsensitiveOption);

    QRegularExpressionMatch match = re.matchView(QStringView(fileName));
    if (!match.hasMatch()) {
        qWarning() << "The name does not match the format";
        return false;
    }

    const QString partStr = match.captured(1).isEmpty() ? "LO" : match.captured(1);
    const QString algoStr = match.captured(2);

    const int day    = match.captured(3).toInt();
    const int month  = match.captured(4).toInt();
    const int year   = match.captured(5).toInt();
    const int hour   = match.captured(6).toInt();
    const int minute = match.captured(7).toInt();
    const int second = match.captured(8).toInt();

    QMetaEnum me = QMetaEnum::fromType<FullAlgoType::Type>();
    int algoVal = me.keyToValue(algoStr.toLatin1().constData());
    if (algoVal == -1) {
        qWarning() << "Unable to extract algo type";
        return false;
    }

    me = QMetaEnum::fromType<PartType::Type>();
    int partVal = me.keyToValue(partStr.toLatin1().constData());
    if (partVal == -1) {
        qWarning() << "Unable to extract part type";
        return false;
    }

    if (!QDate::isValid(year, month, day)) {
        qWarning() << "Invalid Date";
        return false;
    }
    if (!QTime::isValid(hour, minute, second)) {
        qWarning() << "Invalid Time";
        return false;
    }

    if (out != nullptr) {
        out->partType = static_cast<PartType::Type>(partVal);
        out->abbreviation = QString("%1 %2").arg(partStr, algoStr);
        if (out->partType == PartType::LO) {
            out->algoType.full = static_cast<FullAlgoType::Type>(algoVal);
        } else if (out->partType == PartType::GO) {
            out->algoType.base = static_cast<AlgoType::Type>(algoVal);
        }
        out->date = QDate(year, month, day);
        out->time = QTime(hour, minute, second);
    } else {
        qDebug() << "FileData *out is nullptr";
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
