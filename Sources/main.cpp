#include "Reporter.hpp"

#include <CoordinateDescent/CoordinateDescent.hpp>
#include <GradientDescent/GradientDescent.hpp>

#include <muParser.h>

#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <iostream>

double MySqr(double a_fVal) {  return a_fVal*a_fVal; }

void testMuparser()
{
    try
	{
		double var_a = 1;
		mu::Parser p;
		p.DefineVar("a", &var_a); 
		p.DefineFun("MySqr", MySqr); 
		p.SetExpr("MySqr(a)*_pi+min(10,a)");

		for (std::size_t a=0; a<5; ++a)
		{
			var_a = a;  // Change value of variable a
			std::cout << p.Eval() << std::endl;
		}
	}
	catch (mu::Parser::exception_type &e)
	{
		std::cout << e.GetMsg() << std::endl;
	}
}

int main(int argc, char *argv[])
{
    /* ------------- TEST -------------- */

    testMuparser();

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
