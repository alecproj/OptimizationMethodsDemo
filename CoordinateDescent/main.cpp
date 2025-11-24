//
// Created on 25 Oct, 2025
// by alecproj
//
#include <CoordinateDescent/CoordinateDescent.hpp>
#include <CoordinateDescent/Common.hpp>
#include <glog/logging.h>
#include <muParser.h>
#include <iostream>
#include <typeinfo>
#include <locale>
#include <string>
#include <variant>
// #include <windows.h>


using namespace CD;
// Класс-заглушка - реализовывать не нужно!
class MockReporter {
    using Cell = std::variant<std::string, double, long long, bool>;
public:
    int begin() { return 0; }
    int end() { return 0; }
    void insertValue(const std::string &name, double value)
    {
        LOG(WARNING) << name << ": " << value;
    }
    void insertMessage(const std::string &text)
    {
        LOG(WARNING) << text;
    }
    int beginTable(const std::string &title, const std::vector<std::string> &columnHeaders)
    {
        static int tid = 0;
        LOG(WARNING) << "Открыта таблица: " << title;
        std::ostringstream stream;
        for (const auto h : columnHeaders) {
            stream << h << " ";
        }
        LOG(WARNING) << stream.str();
        return ++tid;
    }
    int insertRow(int tableId, const std::vector<Cell> &row)
    {
        std::ostringstream stream;
        for (const auto v : row) {
            std::visit([&stream](auto&& value) {
                stream << value;
            }, v);
            stream << " ";
        }
        LOG(WARNING) << tableId << ": " << stream.str();
        return 0;
    }
    void insertResult(double x, double y, double funcValue)
    {
        LOG(WARNING) << "Ответ: x=" << x << " y=" << y << " f=" << funcValue;
    }
    void endTable(int tableId)
    {
        LOG(WARNING) << "Таблица " << tableId << " закрыта";
    }
};
double MySqr(double a_fVal) { return a_fVal * a_fVal; }
void testMuparser()
{
	try
	{
		double var_a = 1;
		mu::Parser p;
		p.DefineVar("a", &var_a);
		p.DefineFun("MySqr", MySqr);
		p.SetExpr("MySqr(a)*_pi+min(10,a)");
		for (std::size_t a = 0; a < 5; ++a)
		{
			var_a = a; // Change value of variable a
			std::cout << p.Eval() << std::endl;
		}
	}
	catch (mu::Parser::exception_type& e)
	{
		std::cout << e.GetMsg() << std::endl;
	}
}

// Конвертация типа шага в строку
inline std::string stepTypeToString(StepType type) {
    switch (type) {
        case StepType::CONSTANT:    return "Постоянный шаг";
        case StepType::COEFFICIENT: return "Коэффициентный шаг";
        case StepType::ADAPTIVE:    return "Адаптивный шаг";
        default:                    return "Ошибка. неизвестный тип шага";
    }
}

// Конвертация типа алгоритма в строковое сообщение
inline std::string algorithmTypeToString(AlgorithmType type) {
    switch (type) {
        case AlgorithmType::BASIC_COORDINATE_DESCENT:    return "Базовый Алгоритм покоординатного спуска";
        case AlgorithmType::STEEPEST_COORDINATE_DESCENT: return "Алгоритм покоординатного спуска с расишрением (наискорейший спуск)";
        
        default:                                         return "Ошибка. Неизвестный алгоритм";
    }
}

// Конвертация типа экстремума в строку
inline std::string extremumTypeToString(ExtremumType type) {
    switch (type) {
        case ExtremumType::MINIMUM: return "Минимум";
        case ExtremumType::MAXIMUM: return "Максимум";

        default:                    return "Ошибка. неизвестный параметр";
    }
}

AlgorithmType stringToAlgorithmType(const std::string& str) {
    if (str == "BASIC_COORDINATE_DESCENT") return AlgorithmType::BASIC_COORDINATE_DESCENT;
    if (str == "STEEPEST_COORDINATE_DESCENT") return AlgorithmType::STEEPEST_COORDINATE_DESCENT;
    throw std::invalid_argument("Неверный тип алгоритма");
}

ExtremumType stringToExtremumType(const std::string& str) {
    if (str == "MINIMUM") return ExtremumType::MINIMUM;
    if (str == "MAXIMUM") return ExtremumType::MAXIMUM;
    throw std::invalid_argument("Неверный тип экстремума");
}

StepType stringToStepType(const std::string& str) {
    if (str == "CONSTANT") return StepType::CONSTANT;
    if (str == "COEFFICIENT") return StepType::COEFFICIENT;
    if (str == "ADAPTIVE") return StepType::ADAPTIVE;
    throw std::invalid_argument("Неверный тип шага");
}

int main(int argc, char *argv[])
{
    FLAGS_logtostderr = true;
    FLAGS_alsologtostderr = false;
    FLAGS_stderrthreshold = 0;
    FLAGS_log_prefix = false;
    FLAGS_colorlogtostderr = true;
    FLAGS_minloglevel = 0;
    google::InitGoogleLogging(argv[0]);
    google::SetVLOGLevel("*", DEBUG);

	// SetConsoleOutputCP(1251);
	// SetConsoleCP(1251);
	// setlocale(LC_ALL, "Russian");
	// testMuparser(); // Можно закомментировать, если не нужен
	using AlgoType = CoordinateDescent<MockReporter>;
	MockReporter reporter{};
	AlgoType algo{ &reporter };
	InputData data{};
	// Ввод данных от пользователя
	std::cout << "Введите функцию (например, x^2 + y^2): ";
	std::getline(std::cin, data.function);
	std::string input_str;
	try {
		std::cout << "Введите тип алгоритма (BASIC_COORDINATE_DESCENT или STEEPEST_COORDINATE_DESCENT): ";
		std::cin >> input_str;
		data.algorithm_type = stringToAlgorithmType(input_str);
		std::cout << "Введите тип экстремума (MINIMUM или MAXIMUM): ";
		std::cin >> input_str;
		data.extremum_type = stringToExtremumType(input_str);

		// ВВОД ТИПОВ ШАГОВ ДЛЯ КАЖДОЙ КООРДИНАТЫ
		std::cout << "Введите тип шага для X (CONSTANT, COEFFICIENT или ADAPTIVE): ";
		std::cin >> input_str;
		data.step_type_x = stringToStepType(input_str);

		std::cout << "Введите тип шага для Y (CONSTANT, COEFFICIENT или ADAPTIVE): ";
		std::cin >> input_str;
		data.step_type_y = stringToStepType(input_str);

		std::cout << "Введите начальное приближение X: ";
		std::cin >> data.initial_x;
		std::cout << "Введите начальное приближение Y: ";
		std::cin >> data.initial_y;
		std::cout << "Введите левую границу X: ";
		std::cin >> data.x_left_bound;
		std::cout << "Введите правую границу X: ";
		std::cin >> data.x_right_bound;
		std::cout << "Введите левую границу Y: ";
		std::cin >> data.y_left_bound;
		std::cout << "Введите правую границу Y: ";
		std::cin >> data.y_right_bound;
		std::cout << "Введите точность результата (например, 1e-6): ";
		std::cin >> data.result_precision;
		std::cout << "Введите точность вычислений (например, 1e-8): ";
		std::cin >> data.computation_precision;

		// УМНЫЙ ВВОД ПАРАМЕТРОВ ШАГОВ В ЗАВИСИМОСТИ ОТ ВЫБРАННОГО ТИПА

		// Для X - запрашиваем только нужные параметры
		if (data.step_type_x == StepType::CONSTANT || data.step_type_x == StepType::ADAPTIVE) {
			std::cout << "Введите постоянный шаг для X (используется для "
				<< (data.step_type_x == StepType::CONSTANT ? "CONSTANT" : "ADAPTIVE как начальный")
				<< "): ";
			std::cin >> data.constant_step_size_x;
		}
		if (data.step_type_x == StepType::COEFFICIENT) {
			std::cout << "Введите коэффициент шага для X (для COEFFICIENT метода): ";
			std::cin >> data.coefficient_step_size_x;
		}

		// Для Y - запрашиваем только нужные параметры
		if (data.step_type_y == StepType::CONSTANT || data.step_type_y == StepType::ADAPTIVE) {
			std::cout << "Введите постоянный шаг для Y (используется для "
				<< (data.step_type_y == StepType::CONSTANT ? "CONSTANT" : "ADAPTIVE как начальный")
				<< "): ";
			std::cin >> data.constant_step_size_y;
		}
		if (data.step_type_y == StepType::COEFFICIENT) {
			std::cout << "Введите коэффициент шага для Y (для COEFFICIENT метода): ";
			std::cin >> data.coefficient_step_size_y;
		}

		// УБИРАЕМ старый общий коэффициент (он больше не нужен)
		// std::cout << "Введите коэффициент шага (например, 0.01): ";
		// std::cin >> data.coefficient_step_size;

		std::cout << "Введите максимальное число итераций (например, 10000): ";
		std::cin >> data.max_iterations;
		std::cout << "Введите максимальное число вызовов функции (например, 100000): ";
		std::cin >> data.max_function_calls;
	}
	catch (const std::exception& e) {
		std::cout << "Ошибка ввода: " << e.what() << std::endl;
		return 1;
	}
	// Проверка и установка данных (вызовет ошибки из CoordinateDescent, если неверно)
	auto rv = algo.setInputData(&data);
	if (rv != Result::Success) {
		std::cout << "Ошибка установки данных: " << resultToString(rv) << std::endl;
		return 1;
	}
	rv = algo.solve();
	std::cout << "Результат solve: " << resultToString(rv) << std::endl;
	std::cout << "Оптимум в точке: (" << algo.getX() << ", " << algo.getY() << ")" << std::endl;
	std::cout << "Значение функции: " << algo.getOptimumValue() << std::endl;
	std::cout << "Итераций: " << algo.getIterations() << ", Вызовов функции: " << algo.getFunctionCalls() << std::endl;
    google::ShutdownGoogleLogging();
	return 0;
}
