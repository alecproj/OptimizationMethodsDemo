#include "AppEnums.hpp"
#include "InputData.hpp"
#include "MainController.hpp"

#include "Tests/TestReporter.hpp"
#include <glog/logging.h>
#include <muParser.h>

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include <iostream>

#ifndef DEBUG
#define DEBUG 1
#endif

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

    {
        //testMuparser();
        //TestReporter test;
        //test.test();
    }

    /* ------------- /TEST ------------- */
    FLAGS_logtostderr = true;
    FLAGS_alsologtostderr = false;
    FLAGS_stderrthreshold = 0;
    FLAGS_log_prefix = false;
    FLAGS_colorlogtostderr = true;
    FLAGS_minloglevel = MIN_LOG_LEVEL;
    google::InitGoogleLogging(argv[0]);
#ifndef NDEBUG
    google::SetVLOGLevel("*", DEBUG);
#endif

    LOG(INFO) << "Логирование включено";
    VLOG(DEBUG) << "Отладочная сборка";

    QGuiApplication app(argc, argv);

    qmlRegisterUncreatableType<ExtremumType>("AppEnums", 1, 0, "ExtremumType", "Type of extremum");
    qmlRegisterUncreatableType<StepType>("AppEnums", 1, 0, "StepType", "Type of step");
    qmlRegisterUncreatableType<AlgoType>("AppEnums", 1, 0, "AlgoType", "Algo type ID");
    qmlRegisterUncreatableType<ExtensionType>("AppEnums", 1, 0, "ExtensionType", "Extension type ID");
    qmlRegisterUncreatableType<FullAlgoType>("AppEnums", 1, 0, "FullAlgoType", "Full algo type ID");
    qmlRegisterUncreatableType<CheckList>("AppEnums", 1, 0, "CheckList", "Input data check list");
    qmlRegisterUncreatableType<Result>("AppEnums", 1, 0, "Result", "Result of MainController methods");
    qmlRegisterType<EnumHelper>("AppEnums", 1, 0, "EnumHelper");
    qmlRegisterType<InputData>("InputData", 1, 0, "InputData");

    MainController controller{};
    controller.updateQuickInfoModel();
    qDebug() << "Files count: " << controller.quickInfoModel()->count();

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("controller", &controller);

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("OptimizationMethodsDemo", "Main");

    auto rv = app.exec();
    google::ShutdownGoogleLogging();
    return rv;
}
