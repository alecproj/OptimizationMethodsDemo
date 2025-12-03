#ifndef SOURCES_TESTS_TESTREPORTER_HPP_
#define SOURCES_TESTS_TESTREPORTER_HPP_

#include "ReportWriter.hpp"
#include "LOInputData.hpp"
#include "GOInputData.hpp"
#include "AppEnums.hpp"
#include <QDebug>

using namespace std;

class TestReporter {
    ReportWriter reporter;
    InputData *data;
public:
    void test()
    {
        testData2();
        reporter.setInputData(data);
        auto rv = reporter.begin();
        if (rv != 0) {
            delete data;
            data = nullptr;
            qCritical() << "TEST FAIL: reporter.begin: " << rv;
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
            delete data;
            data = nullptr;
            qCritical() << "TEST FAIL: reporter.end: " << rv;
            return;
        }
        qDebug() << "TEST SUCCESS: ура";
        delete data;
        data = nullptr;
    }
private:
    inline void testData1()
    {
        data = new LO::InputData();
        auto lodata = static_cast<LO::InputData *>(data);
        lodata->setFunction("4*x - x^2 - y^2 + 2*y + 5");
        lodata->setAlgorithmId(AlgoType::CD);
        lodata->setExtensionId(LO::ExtensionType::B);
        lodata->setFullAlgoId(LO::FullAlgoType::CDB);
        lodata->setExtremumId(ExtremumType::MAXIMUM);
        lodata->setStepId(LO::StepType::COEFFICIENT);
        lodata->setStartX1(0.0);
        lodata->setStartY1(0.0);
        lodata->setMinX(-10.0);
        lodata->setMaxX(10.0);
        lodata->setMinY(-10.0);
        lodata->setMaxY(10.0);
        lodata->setResultAccuracy(6);
        lodata->setCalcAccuracy(8);
        lodata->setStepX(0.1);
        lodata->setStepY(0.1);
        lodata->setMaxIterations(100);
        lodata->setMaxFuncCalls(1000);
    }

    inline void testData2()
    {
        data = new GO::InputData();
        auto godata = static_cast<GO::InputData *>(data);
        godata->setFunction("4*x - x^2 - y^2 + 2*y + 5");
        godata->setAlgorithmId(AlgoType::GA);
        godata->setExtremumId(ExtremumType::MAXIMUM);
        godata->setMinX(-10.0);
        godata->setMaxX(10.0);
        godata->setMinY(-10.0);
        godata->setMaxY(10.0);
        godata->setResultAccuracy(6);
        godata->setCalcAccuracy(8);
        godata->setMaxIterations(100);
        godata->setSize(125);
        godata->setElitism(5);
        godata->setCrossoverProb(0.7);
        godata->setMutationProb(0.05);
    }

    inline void testData3()
    {
        data = new GO::InputData();
        auto godata = static_cast<GO::InputData *>(data);
        godata->setFunction("4*x - x^2 - y^2 + 2*y + 5");
        godata->setAlgorithmId(AlgoType::PS);
        godata->setExtremumId(ExtremumType::MINIMUM);
        godata->setMinX(-10.0);
        godata->setMaxX(10.0);
        godata->setMinY(-10.0);
        godata->setMaxY(10.0);
        godata->setResultAccuracy(6);
        godata->setCalcAccuracy(8);
        godata->setMaxIterations(100);
        godata->setSize(125);
        godata->setInertiaCoef(0.5);
        godata->setCognitiveCoef(1.0);
        godata->setSocialCoef(2.0);
    }
};

#endif // SOURCES_TESTS_TESTREPORTER_HPP_
