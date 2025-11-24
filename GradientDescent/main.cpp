//
// Created on 25 Oct, 2025
// by alecproj
//
#include <GradientDescent/GradientDescent.hpp>
#include <GradientDescent/Common.hpp>
#include <glog/logging.h>
#include <muParser.h>
#include <iostream>
#include <typeinfo>
#include <string>
#include <variant>

using namespace GD;
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

double MySqr(const double a_fVal) { return a_fVal * a_fVal; }

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
    case AlgorithmType::GRADIENT_DESCENT:    return "Базовый градиентный спуск";
    case AlgorithmType::STEEPEST_DESCENT:    return "Наискорейший градиентный спуск";
    case AlgorithmType::RAVINE_METHOD:       return "Овражный метод для градиентного спуска";
    default:                                 return "Ошибка. Неизвестный алгоритм";
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

// Вспомогательные функции для конвертации строк в enum
AlgorithmType stringToAlgorithmType(const std::string& str) {
    if (str == "GRADIENT_DESCENT") return AlgorithmType::GRADIENT_DESCENT;
    if (str == "STEEPEST_DESCENT") return AlgorithmType::STEEPEST_DESCENT;
    if (str == "RAVINE_METHOD") return AlgorithmType::RAVINE_METHOD;
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
    // google::SetVLOGLevel("*", DEBUG);

    using AlgoType = GradientDescent<MockReporter>;
    MockReporter reporter{};
    AlgoType algo{ &reporter };
    InputData data{};

    // Ввод данных от пользователя
    std::cout << "Введите функцию (например, x^2 + y^2): ";
    std::getline(std::cin, data.function);
    std::string input_str;

    try {
        // ВВОД ТИПА АЛГОРИТМА
        std::cout << "Введите тип алгоритма (GRADIENT_DESCENT, STEEPEST_DESCENT, RAVINE_METHOD): ";
        std::cin >> input_str;
        data.algorithm_type = stringToAlgorithmType(input_str);

        std::cout << "Введите тип экстремума (MINIMUM или MAXIMUM): ";
        std::cin >> input_str;
        data.extremum_type = stringToExtremumType(input_str);

        // НАСТРОЙКИ ГРАДИЕНТНЫХ МЕТОДОВ
        std::cout << "=== НАСТРОЙКИ ГРАДИЕНТНОГО МЕТОДА ===" << std::endl;

        // Для STEEPEST_DESCENT и RAVINE_METHOD шаг не нужен - он подбирается автоматически
        if (data.algorithm_type == AlgorithmType::STEEPEST_DESCENT) {
            std::cout << "Для STEEPEST_DESCENT шаг подбирается автоматически методом золотого сечения" << std::endl;
            // Устанавливаем дефолтные значения (не используются в алгоритме)
            data.step_type = StepType::CONSTANT;
            data.constant_step_size = 0.1; // Начальное значение для поиска
        }
        else if (data.algorithm_type == AlgorithmType::RAVINE_METHOD) {
            std::cout << "Для RAVINE_METHOD шаг подбирается адаптивно для работы с овражными функциями" << std::endl;
            std::cout << "Метод автоматически детектирует овражные участки и меняет стратегию" << std::endl;
            // Устанавливаем дефолтные значения
            data.step_type = StepType::CONSTANT;
            data.constant_step_size = 0.1; // Базовый шаг
        }
        else {
            // Для GRADIENT_DESCENT запрашиваем тип шага
            std::cout << "Введите тип шага (CONSTANT, COEFFICIENT или ADAPTIVE): ";
            std::cin >> input_str;
            data.step_type = stringToStepType(input_str);

            // Запрашиваем параметры шага
            if (data.step_type == StepType::CONSTANT || data.step_type == StepType::ADAPTIVE) {
                std::cout << "Введите постоянный шаг (используется для "
                    << (data.step_type == StepType::CONSTANT ? "CONSTANT" : "ADAPTIVE как начальный")
                    << "): ";
                std::cin >> data.constant_step_size;
            }
            if (data.step_type == StepType::COEFFICIENT) {
                std::cout << "Введите коэффициент шага (для COEFFICIENT метода): ";
                std::cin >> data.coefficient_step_size;
            }
        }

        // ОБЩИЕ ПАРАМЕТРЫ ДЛЯ ВСЕХ АЛГОРИТМОВ
        std::cout << "=== ОБЩИЕ ПАРАМЕТРЫ ===" << std::endl;
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

        std::cout << "Введите максимальное число итераций (например, 10000): ";
        std::cin >> data.max_iterations;
        std::cout << "Введите максимальное число вызовов функции (например, 100000): ";
        std::cin >> data.max_function_calls;

        // ВЫВОД ИНФОРМАЦИИ О ВЫБРАННОМ АЛГОРИТМЕ
        std::cout << std::endl << "=== ВЫБРАННЫЙ АЛГОРИТМ ===" << std::endl;
        switch (data.algorithm_type) {
        case AlgorithmType::GRADIENT_DESCENT:
            std::cout << "Базовый градиентный спуск" << std::endl;
            std::cout << "- Движение по всем координатам одновременно" << std::endl;
            std::cout << "- Один шаг для всех направлений" << std::endl;
            break;
        case AlgorithmType::STEEPEST_DESCENT:
            std::cout << "Наискорейший спуск" << std::endl;
            std::cout << "- Оптимальный шаг вдоль направления градиента" << std::endl;
            std::cout << "- Метод золотого сечения для подбора шага" << std::endl;
            std::cout << "- Автоматическая настройка шага на каждой итерации" << std::endl;
            break;
        case AlgorithmType::RAVINE_METHOD:
            std::cout << "Метод овражных функций" << std::endl;
            std::cout << "- Специально для функций с овражным рельефом" << std::endl;
            std::cout << "- Автоматическая детекция оврагов" << std::endl;
            std::cout << "- Адаптивная стратегия для разных участков" << std::endl;
            std::cout << "- Движение вдоль 'дна оврага' в сложных участках" << std::endl;
            std::cout << "- Идеально для функций типа x² + 100*y² или Розенброка" << std::endl;
            break;
        }
        std::cout << "=========================" << std::endl << std::endl;

    }
    catch (const std::exception& e) {
        std::cout << "Ошибка ввода: " << e.what() << std::endl;
        return 1;
    }

    // Проверка и установка данных (вызовет ошибки из GradientDescent, если неверно)
    auto rv = algo.setInputData(&data);
    if (rv != Result::Success) {
        std::cout << "Ошибка установки данных: " << resultToString(rv) << std::endl;
        return 1;
    }

    // Запуск алгоритма
    rv = algo.solve();

    // Вывод результатов
    std::cout << std::endl << "=== РЕЗУЛЬТАТЫ ===" << std::endl;
    std::cout << "Результат solve: " << resultToString(rv) << std::endl;
    std::cout << "Оптимум в точке: (" << algo.getX() << ", " << algo.getY() << ")" << std::endl;
    std::cout << "Значение функции: " << algo.getOptimumValue() << std::endl;
    std::cout << "Итераций: " << algo.getIterations() << ", Вызовов функции: " << algo.getFunctionCalls() << std::endl;

    google::ShutdownGoogleLogging();
    return 0;
}
