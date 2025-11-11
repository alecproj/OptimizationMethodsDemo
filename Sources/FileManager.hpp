//
// Created on 3 Nov, 2025
//  by alecproj
//

#ifndef SOURCES_FILEMANAGER_HPP_
#define SOURCES_FILEMANAGER_HPP_

#include <QDir>
#include <QString>
#include <QJsonObject>

class FileManager {

public:

    static bool ensureBaseDirExists(QString &outDir);
    static QStringList listFiles(
        QDir::SortFlags sortBy = (QDir::Time | QDir::Reversed),
        const QString &filter = QString()
    );
    static bool fileExists(const QString &fileName);
    static bool loadJsonFile(const QString &fileName, QJsonObject &outObj);
    static bool saveJsonFile(const QString &fileName, const QJsonObject &obj);
    static bool deleteFile(const QString &fileName);
    // { "size": qint64, "lastModified": QString (ISO), "isReadable": bool, "isWritable": bool }
    static QVariantMap fileInfo(const QString &fileName);

    static uint32_t crc32FromBytes(const QByteArray &bytes);

protected:

    static QString baseDir();
};

#endif // SOURCES_FILEMANAGER_HPP_
