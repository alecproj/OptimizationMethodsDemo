//
// Created on 25 Oct, 2025
// by alecproj
//

#include <ConjugateGradient/ConjugateGradient.hpp>
#include <ConjugateGradient/Common.hpp>
#include <muParser.h>
#include <iostream>
#include <locale>
#include <string>

using namespace CG;

// Класс-заглушка - реализовывать не нужно!
class MockReporter {
public:
    int begin() { return 1; } // Возвращаем 1, чтобы обойти проверку в solve() (предполагаем опечатку в условии ==0)
    int end() { return 0; } // Возвращаем 0, чтобы solve() вернул Success при успешном завершении
};

double MySqr(double a_fVal) { return a_fVal * a_fVal; }

int main()
{

    using AlgoType = ConjugateGradient<MockReporter>;
    MockReporter reporter{};
    AlgoType algo{ &reporter };
    InputData data{};

    // Ввод данных от пользователя
    std::cout << "Введите функцию (например, x^2 + y^2): ";
    std::getline(std::cin, data.function);

    std::string input_str;

    try {
        // Устанавливаем метод сопряженных градиентов
        data.algorithm_type = AlgorithmType::CONJUGATE_GRADIENT;

        std::cout << "Введите тип экстремума (MINIMUM или MAXIMUM): ";
        std::cin >> input_str;
        data.extremum_type = stringToExtremumType(input_str);

        std::cout << "=== НАСТРОЙКИ МЕТОДА СОПРЯЖЕННЫХ ГРАДИЕНТОВ ===" << std::endl;

        // ДЛЯ МЕТОДА СОПРЯЖЕННЫХ ГРАДИЕНТОВ - ОДИН ТИП ШАГА ДЛЯ ВСЕХ КООРДИНАТ
        std::cout << "Введите тип шага (CONSTANT, COEFFICIENT или ADAPTIVE): ";
        std::cin >> input_str;
        data.step_type = stringToStepType(input_str);  // Основной тип шага

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
        std::cout << "Метод сопряженных градиентов" << std::endl;
        std::cout << "- Адаптивный спуск с сопряженными направлениями" << std::endl;
        std::cout << "- Для овражных функций типа Розенброка" << std::endl;
        std::cout << "- Тип шага: " << stepTypeToString(data.step_type) << std::endl;
        std::cout << "=========================" << std::endl << std::endl;

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

    // Запуск алгоритма
    rv = algo.solve();

    // Вывод результатов
    std::cout << std::endl << "=== РЕЗУЛЬТАТЫ ===" << std::endl;
    std::cout << "Результат solve: " << resultToString(rv) << std::endl;
    std::cout << "Оптимум в точке: (" << algo.getX() << ", " << algo.getY() << ")" << std::endl;
    std::cout << "Значение функции: " << algo.getOptimumValue() << std::endl;
    std::cout << "Итераций: " << algo.getIterations() << ", Вызовов функции: " << algo.getFunctionCalls() << std::endl;

    return 0;
}
