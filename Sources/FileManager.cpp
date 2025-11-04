//
// Created on 3 Nov, 2025
//  by alecproj
//

#include "FileManager.hpp"

#include <QStandardPaths>
#include <QSaveFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLockFile>
#include <QFile>
#include <QDebug>

bool FileManager::ensureBaseDirExists(QString &outDir)
{
    outDir = baseDir();
    QDir dir;
    if (dir.exists(outDir)) return true;
    if (!dir.mkpath(outDir)) {
        qWarning() << "FileManager: cannot create directory:" << outDir;
        return false;
    }
    return true;
}

QStringList FileManager::listFiles(QDir::SortFlag sortBy, const QString &filter)
{
    QString path{};
    if (!ensureBaseDirExists(path)) {
         return {};
    }
    QDir dir(path);
    QStringList nameFilters;
    if (filter.isEmpty()) nameFilters << QStringLiteral("*.json");
    else nameFilters << filter;
    return dir.entryList(nameFilters, QDir::Files | QDir::Readable, sortBy);
}

bool FileManager::fileExists(const QString &fileName)
{
    QString path{};
    if (!ensureBaseDirExists(path)) {
        return false;
    }
    QFileInfo fi(QDir(path).filePath(fileName));
    return fi.exists() && fi.isFile();
}

bool FileManager::loadJsonFile(const QString &fileName, QJsonObject &outObj)
{
    QString dir;
    if (!ensureBaseDirExists(dir)) return {};

    QString fullPath = QDir(dir).filePath(fileName);

    QFile f(fullPath);
    if (!f.open(QIODevice::ReadOnly)) {
        qWarning() << "FileManager: cannot open file for reading:" << fullPath;
        return false;
    }
    QByteArray data = f.readAll();
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(data, &err);
    if (err.error != QJsonParseError::NoError) {
        qWarning() << "FileManager: JSON parse error in:" << fullPath << ":" << err.errorString();
        return false;
    }
    if (!doc.isObject()) {
        qWarning() << "FileManager: JSON object expected in:" << fullPath;
        return false;
    }
    outObj = doc.object();
    return true;
}

bool FileManager::saveJsonFile(const QString &fileName, const QJsonObject &obj)
{
    QString dir;
    if (!ensureBaseDirExists(dir)) return false;

    QString fullPath = QDir(dir).filePath(fileName);

    QLockFile lock(fullPath + ".lock");
    lock.setStaleLockTime(30000); // 30 s
    if (!lock.tryLock(5000)) { // wait during 5s
        qWarning() << "FileManager: cannot lock file for writing:" << fullPath;
        return false;
    }

    QSaveFile file(fullPath);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "FileManager: cannot open file for writing:" << fullPath << file.errorString();
        return false;
    }

    QJsonDocument doc(obj);
    QByteArray data = doc.toJson(QJsonDocument::Indented);
    if (file.write(data) != data.size()) {
        qWarning() << "FileManager: incomplete write to" << fullPath;
        file.cancelWriting();
        return false;
    }
    if (!file.commit()) {
        qWarning() << "FileManager: commit failed for" << fullPath;
        return false;
    }
    return true;
}

bool FileManager::deleteFile(const QString &fileName) {
    QString path;
    if (!ensureBaseDirExists(path)) {
        return false;
    }
    QString fullPath = QDir(path).filePath(fileName);

    // Lock to avoid racing with writer
    QLockFile lock(fullPath + QLatin1String(".lock"));
    lock.setStaleLockTime(30000);
    if (!lock.tryLock(2000)) {
        qWarning() << "FileManager: cannot lock file for deletion:" << fullPath;
        return false;
    }

    QFile f(fullPath);
    if (!f.exists()) {
        return false;
    }
    if (!f.remove()) {
        qWarning() << "FileManager: failed to remove" << fullPath;
        return false;
    }
    return true;
}

QVariantMap FileManager::fileInfo(const QString &fileName)
{
    QString path;
    QVariantMap m;
    if (!ensureBaseDirExists(path)) {
        return m;
    }
    QFileInfo fi(QDir(path).filePath(fileName));
    if (!fi.exists()) return m;
    m.insert(QStringLiteral("size"), QVariant::fromValue(fi.size()));
    m.insert(QStringLiteral("lastModified"), fi.lastModified().toString(Qt::ISODate));
    m.insert(QStringLiteral("isReadable"), fi.isReadable());
    m.insert(QStringLiteral("isWritable"), fi.isWritable());
    return m;
}

uint32_t FileManager::crc32FromBytes(const QByteArray &bytes)
{
    static uint32_t table[256];
    static bool inited = false;
    if (!inited) {
        for (uint32_t i = 0; i < 256; ++i) {
            uint32_t c = i;
            for (int j = 0; j < 8; ++j)
                c = (c & 1) ? (0xEDB88320u ^ (c >> 1)) : (c >> 1);
            table[i] = c;
        }
        inited = true;
    }

    uint32_t crc = 0xFFFFFFFFu;
    for (unsigned char ch : bytes) {
        crc = table[(crc ^ ch) & 0xFFu] ^ (crc >> 8);
    }
    return crc ^ 0xFFFFFFFFu;
}

QString FileManager::baseDir()
{
    QString base = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString dir = QDir(base).filePath(QStringLiteral("OptimizationMethodsDemo/Reports"));
    return dir;
}

