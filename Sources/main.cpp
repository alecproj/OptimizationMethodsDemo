#include "Reporter.hpp"

#include <CoordinateDescent/CoordinateDescent.hpp>
#include <GradientDescent/GradientDescent.hpp>

#include <QGuiApplication>
#include <QQmlApplicationEngine>

int main(int argc, char *argv[])
{
    /* ------------- TEST -------------- */

    using GDAlgoType = GradientDescent<Reporter>;
    using CDAlgoType = CoordinateDescent<Reporter>;
    Reporter reporter{};

    GDAlgoType gdAlgo{&reporter};
    GradientInput gdData{};

    auto gdRv = gdAlgo.setInputData(&gdData);
    if (gdRv == GDResult::Success) {
        gdRv = gdAlgo.solve();
    }

    CDAlgoType cdAlgo{&reporter};
    CoordinateInput cdData{};

    auto cdRv = cdAlgo.setInputData(&cdData);
    if (cdRv == CDResult::Success) {
        cdRv = cdAlgo.solve();
    }

    /* ------------- /TEST ------------- */
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("OptimizationMethodsDemo", "Main");

    return app.exec();
}
