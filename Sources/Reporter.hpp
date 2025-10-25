//
// Created on 25 Oct, 2025
//  by alecproj
//

#ifndef SOURCES_REPORTER_HPP_
#define SOURCES_REPORTER_HPP_

#include <QObject>

class Reporter : public QObject {
    Q_OBJECT

public:
    explicit Reporter(QObject* parent = nullptr);
};

#endif // SOURCES_REPORTER_HPP_
