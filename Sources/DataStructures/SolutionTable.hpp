//
// Created on 29 Nov, 2025
//  by alecproj
//

#ifndef SOURCES_SOLUTIONTABLE_HPP_
#define SOURCES_SOLUTIONTABLE_HPP_

#include "SolutionTableModel.hpp"
#include "SolutionMessage.hpp"
#include <QObject>
#include <QString>

class SolutionTable : public SolutionMessage {
    Q_OBJECT
    Q_PROPERTY(SolutionTableModel *table READ table CONSTANT)

public:
    explicit SolutionTable(SolutionTableModel *table, const QString &text, QObject *parent = nullptr)
        : SolutionMessage(text, parent)
        , m_table(table)
    {
    }

    QString type() const override { return "table"; };
    SolutionTableModel *table() const { return m_table; }
private:
    SolutionTableModel *m_table;
};

#endif // SOURCES_SOLUTIONTABLE_HPP_
