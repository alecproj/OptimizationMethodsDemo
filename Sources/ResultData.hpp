//
// Created on 9 Nov, 2025
//  by alecproj
//

#ifndef SOURCES_RESULTDATA_HPP_
#define SOURCES_RESULTDATA_HPP_

#include "AppEnums.hpp"
#include <QObject>
#include <QString>

class ResultData : public QObject
{
    Q_OBJECT

    Q_PROPERTY(double xValue READ xValue WRITE setXValue NOTIFY xValueChanged)
    Q_PROPERTY(double yValue READ yValue WRITE setYValue NOTIFY yValueChanged)
    Q_PROPERTY(double funcValue READ funcValue WRITE setFuncValue NOTIFY funcValueChanged)
public:
    explicit ResultData(QObject* parent = nullptr)
        : QObject(parent)
        , m_x{0.0}
        , m_y{0.0}
        , m_func{0.0}
    {}

    double xValue() const { return m_x; }
    double yValue() const { return m_y; }
    double funcValue() const { return m_func; }

public slots:
    void setXValue(double v)
    {
        if (m_x != v) {
            m_x = v;
            emit xValueChanged();
        }
    }

    void setYValue(double v)
    {
        if (m_y != v) {
            m_y = v;
            emit yValueChanged();
        }
    }

    void setFuncValue(double v)
    {
        if (m_func != v) {
            m_func = v;
            emit funcValueChanged();
        }
    }

signals:
    void xValueChanged();
    void yValueChanged();
    void funcValueChanged();

private:
    double m_x;
    double m_y;
    double m_func;
};

#endif // SOURCES_RESULTDATA_HPP_
