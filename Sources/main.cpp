#include "AppEnums.hpp"
#include "FileManager.hpp"
#include "MainController.hpp"

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

QString shortFunctionName(const char* func)
{
    if (!func)
        return "unknown";

    QString f = func;

    int paren = f.indexOf('(');
    if (paren > 0)
        f = f.left(paren);

    int lastSpace = f.lastIndexOf(' ');
    if (lastSpace > 0)
        f = f.mid(lastSpace + 1);

    return f;
}

void msgHandler(QtMsgType type, const QMessageLogContext& ctx, const QString& msg)
{
    QString func = shortFunctionName(ctx.function);
    const char* color;
    const char* t;

    switch (type) {
    case QtDebugMsg:
        t = "DEBUG";
        color = "";
        break; // белый
    case QtInfoMsg:
        t = "INFO";
        color = "\033[36m";
        break; // голубой
    case QtWarningMsg:
        t = "WARNING";
        color = "\033[33m";
        break; // желтый
    case QtCriticalMsg:
        t = "CRITICAL";
        color = "\033[31m";
        break; // красный
    case QtFatalMsg:
        t = "FATAL";
        color = "\033[41m";
        break; // красный фон
    }

    fprintf(stderr, "%s[%s] %s: %s\033[0m\n",
        color,
        t,
        func.toUtf8().constData(),
        msg.toUtf8().constData()
    );

    if (type == QtFatalMsg)
        abort();
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

    LOG(INFO) << "GLOG: Логирование включено";
    VLOG(DEBUG) << "GLOG: Отладочная сборка";

    QGuiApplication app(argc, argv);
    qInstallMessageHandler(msgHandler);
    qDebug() << "Отладочная сборка";
    qmlRegisterUncreatableType<ExtremumType>("AppEnums", 1, 0, "ExtremumType", "Type of extremum");
    qmlRegisterUncreatableType<AlgoType>("AppEnums", 1, 0, "AlgoType", "Algo type ID");
    qmlRegisterUncreatableType<Result>("AppEnums", 1, 0, "Result", "Result of MainController methods");
    qmlRegisterType<EnumHelper>("AppEnums", 1, 0, "EnumHelper");
    qmlRegisterType<PartType>("AppEnums", 1, 0, "PartType");
    qmlRegisterUncreatableType<LO::ExtensionType>("AppEnums", 1, 0, "ExtensionType", "Extension type ID");
    qmlRegisterUncreatableType<LO::StepType>("AppEnums", 1, 0, "StepType", "Type of step");
    qmlRegisterUncreatableType<LO::FullAlgoType>("AppEnums", 1, 0, "FullAlgoType", "Full algo type ID");
    qmlRegisterUncreatableType<LO::CheckList>("LOInputData", 1, 0, "CheckList", "LO input data check list");
    qmlRegisterType<LO::InputData>("LOInputData", 1, 0, "InputData");
    qmlRegisterUncreatableType<GO::CheckList>("GOInputData", 1, 0, "CheckList", "GO input data check list");
    qmlRegisterType<GO::InputData>("GOInputData", 1, 0, "InputData");

    MainController controller{};
    qDebug() << "Files count: " << FileManager::listFiles().count();

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
