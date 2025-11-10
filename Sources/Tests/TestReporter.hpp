#ifndef SOURCES_TESTS_TESTREPORTER_HPP_
#define SOURCES_TESTS_TESTREPORTER_HPP_

#include "ReportWriter.hpp"
#include "AppEnums.hpp"
#include <QDebug>

using namespace std;

class TestReporter {
    ReportWriter reporter;
    InputData data;
public:
    void test()
    {
        testData1();
        reporter.setInputData(&data);
        auto rv = reporter.begin();
        if (rv != 0) {
            qDebug() << "TEST FAIL: reporter.begin: " << rv;
            return;
        }

        reporter.insertMessage("Начало заполнения отчета. Просто длинное сообщение чтобы было. Сейчас пойдут две таблицы и значение.");

        auto stid = reporter.beginTable("Маленькая таблица", {"A", "B", "C", "D", "E", "F", "G"});
        reporter.insertValue("Ответ должен быть такой", 10);
        auto btid = reporter.beginTable("Большая таблица", {"Первая колонка", "Вторая колонка", "Третья колонка"});
        reporter.insertRow(stid, {1, 2, 3, 4, 5, 6, 7});
        reporter.insertRow(btid, {"Проверка ограничений на длину ячейки", 0.999999999999999, 999999999999999.1});
        reporter.insertRow(stid, {"раз", "два", "три", "четыре", "пять", "шесть", "семь"});
        reporter.insertRow(stid, {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0});
        reporter.insertRow(stid, {true, false, true, false, true, false, true});
        reporter.insertRow(btid, {"", "Теперь проверка другой ячейки", 0});
        reporter.insertMessage("Закончил писать таблицы");
        reporter.endTable(btid);
        reporter.endTable(stid);
        reporter.insertResult(2.0, 1.0, 10.0);

        reporter.insertValue("Еще одна проверка на вшивость. Вот число", -1);
        rv = reporter.end();
        if (rv != 0) {
            qDebug() << "TEST FAIL: reporter.end: " << rv;
            return;
        }
        qDebug() << "TEST SUCCESS: ура";
    }
private:
    inline void testData1()
    {
        data.setFunction("4*x - x^2 - y^2 + 2*y + 5");
        data.setAlgorithmId(AlgoType::CD);
        data.setExtensionId(ExtensionType::B);
        data.setFullAlgoId(FullAlgoType::CDB);
        data.setExtremumId(ExtremumType::MAXIMUM);
        data.setStartX1(0.0);
        data.setStartY1(0.0);
        data.setMinX(-10.0);
        data.setMaxX(10.0);
        data.setMinY(-10.0);
        data.setMaxY(10.0);
        data.setResultAccuracy(0.000001);
        data.setCalcAccuracy(0.00000001);
        data.setStepX(0.1);
        data.setStepY(0.1);
        data.setMaxIterations(100);
    }
};

#endif // SOURCES_TESTS_TESTREPORTER_HPP_
