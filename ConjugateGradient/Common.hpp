//
// Created on 25 Oct, 2025
//  by alecproj
//

#ifndef CONJUGATEGRADIENT_COMMON_HPP_
#define CONJUGATEGRADIENT_COMMON_HPP_

#include <string>
#include <vector>
#include <stdexcept>

// ============================================================================
// Перечисления состояний и ошибок
// ============================================================================
namespace CG {
    // Результат выполнения алгоритма
    enum class Result : int {
        Success = 0,                       // Успешное выполнение
        Fail = -1,                         // Общая ошибка
        InvalidInput = -2,                 // Неверные входные данные
        NoConvergence = -3,                // Нет сходимости
        OutOfBounds = -4,                  // Выход за границы
        MaxIterations = -5,                // Превышение макс. числа итераций
        MaxFunctionsCalls = -6,            // Превышение макс. числа вызовов функции
        ParseError = -7,                   // Ошибка парсинга функции
        ComputeError = -8,                 // Вычислительная ошибка
        NonDifferentiableFunction = -9,    // Функция не явл. дифференцируемой
        EmptyFunction = -10,               // Функция пустая
        InvalidAlgorithmType = -11,        // Неверный ввод типа алгоритма
        InvalidExtremumType = -12,         // Неверный ввод типа экстремума
        InvalidStepType = -13,             // Неверный ввод типа шага
        InvalidXBound = -14,               // Неверный ввод границ X
        InvalidYBound = -15,               // Неверный ввод границ Y
        InvalidInitialX = -16,             // Неверный ввод начального приближения X
        InvalidInitialY = -17,             // Неверный ввод начального приближения Y
        InvalidResultPrecision = -18,      // Неверный ввод точности результата
        InvalidComputationPrecision = -19, // Неверный ввод точности вычислений
        InvalidLogicPrecision = -20,       // Неверный ввод точностей
        InvalidConstantStepSize = -21,     // Неверный ввод постоянного шага
        InvalidCoefficientStepSize = -22,  // Неверный ввод коэффициентного шага
    };

    // Тип алгоритма оптимизации
    enum class AlgorithmType {
        CONJUGATE_GRADIENT
    };

    // Тип экстремума
    enum class ExtremumType {
        MINIMUM, // Минимум
        MAXIMUM  // Максимум
    };

    // Тип шага в алгоритме
    enum class StepType {
        CONSTANT,    // Постоянный шаг:     step = const;
        COEFFICIENT, // Коэффициентный шаг: step = k * производная
        ADAPTIVE     // Адаптивный шаг:     step подбирается автоматически на каждой итерации
    };

    // ============================================================================
    // Структуры входных данных
    // ============================================================================
    double gradient_epsilon = 1e-8;     // Точность вычисления градиента 

    // Основные входные параметры для алгоритма
    struct InputData {

        // --- ОБЯЗАТЕЛЬНЫЕ ПАРАМЕТРЫ ---
        std::string function;         // Функция для оптимизации
        AlgorithmType algorithm_type; // Тип алгоритма
        ExtremumType extremum_type;   // Тип экстремума
        StepType step_type;           // Тип шага

        // --- НАЧАЛЬНЫЕ УСЛОВИЯ ---
        double initial_x = 0.0; // Начальное приближение X
        double initial_y = 0.0; // Начальное приближение Y

        // --- ГРАНИЦЫ ПОИСКА ---
        double x_left_bound = -1000.0; // Левая граница диапазона X
        double x_right_bound = 1000.0; // Правая граница диапазона X
        double y_left_bound = -1000.0; // Левая граница диапазона Y
        double y_right_bound = 1000.0; // Правая граница диапазона Y


        // --- ПАРАМЕТРЫ ТОЧНОСТИ ---
        double result_precision = 1e-06;     // Точность результата
        double computation_precision = 1e-8; // Точность вычислений

        // --- ПАРАМЕТРЫ ШАГА ---
        double constant_step_size = 0.1;     // Размер постоянного шага
        double coefficient_step_size = 0.01; // Коэффициент шага

        // --- ОГРАНИЧЕНИЯ ---
        int max_iterations = 1000;       // Макс. число итераций 
        int max_function_calls = 10000;  // Макс. число вызовов функции

    };

    // ============================================================================
    // Структура выходных данных
    // ============================================================================

    // Результат оптимизации
    struct ConjugateResult {
        Result status = Result::Success;   // Статус выполнения
        std::vector<double> optimum_point; // Точка экстремума [X, Y]
        double optimum_value;              // Значение функции в точке экстремума
        int iterations = 0;                // Кол-во выполненных итераций
        int function_calls = 0;            // Кол-во вызовов функции
        std::string message;               // Сообщение о результате
    };

    // ============================================================================
    // Вспомогательные функции
    // ============================================================================

    // Конвертация результата алгоритма в строковое сообщение
    inline std::string resultToString(Result result) {
        switch (result) {
        case Result::Success:                     return "Успешно";
        case Result::Fail:                        return "Произошла ошибка";
        case Result::InvalidInput:                return "Некорректные входные данные";
        case Result::NoConvergence:               return "Алгоритм не сходится";
        case Result::OutOfBounds:                 return "Выход за границы";
        case Result::MaxIterations:               return "Достигнут максимум итераций";
        case Result::MaxFunctionsCalls:           return "Достигнут максимум вызовов функции";
        case Result::ParseError:                  return "Ошибка обработки функции";
        case Result::ComputeError:                return "Вычислительная ошибка";
        case Result::NonDifferentiableFunction:   return "Функция не является дифференцируемой";
        case Result::EmptyFunction:               return "Функция простая";
        case Result::InvalidAlgorithmType:        return "Неверный ввод типа алгоритма";
        case Result::InvalidExtremumType:         return "Неверный ввод типа экстремума";
        case Result::InvalidStepType:             return "Неверный ввод типа шага";
        case Result::InvalidXBound:               return "Неверный ввод границ X";
        case Result::InvalidYBound:               return "Неверный ввод границ Y";
        case Result::InvalidInitialX:             return "Неверный ввод начального приближения X";
        case Result::InvalidInitialY:             return "Неверный ввод начального приближения Y";
        case Result::InvalidResultPrecision:      return "Неверный ввод точности результата";
        case Result::InvalidComputationPrecision: return "Неверный ввод точности вычислений";
        case Result::InvalidLogicPrecision:       return "Неверный ввод точностей";
        case Result::InvalidConstantStepSize:     return "Неверный ввод постоянного шага";
        case Result::InvalidCoefficientStepSize:  return "Неверный ввод коэффициентного шага";
        default:                                  return "Unknown result";
        }
    }

    // Конвертация типа шага в строку
    inline std::string stepTypeToString(StepType type) {
        switch (type) {
        case StepType::CONSTANT:    return "Постоянный шаг";
        case StepType::COEFFICIENT: return "Коэффициентный шаг";
        case StepType::ADAPTIVE:    return "Адаптивный шаг";
        default:                    return "Неизвестный тип шага";
        }
    }

    // Конвертация типа алгоритма в строковое сообщение
    inline std::string algorithmTypeToString(AlgorithmType type) {
        switch (type) {
        case AlgorithmType::CONJUGATE_GRADIENT:    return "Метод сопряженных градиентов";
        default:                                   return "Неизвестный алгоритм";
        }
    }

    // Конвертация типа экстремума в строку
    inline std::string extremumTypeToString(ExtremumType type) {
        switch (type) {
        case ExtremumType::MINIMUM: return "Минимум";
        case ExtremumType::MAXIMUM: return "Максимум";

        default:                    return "Неизвестный параметр";
        }
    }

    // Вспомогательные функции для конвертации строк в enum
    inline AlgorithmType stringToAlgorithmType(const std::string& str) {
        if (str == "CONJUGATE_GRADIENT") return AlgorithmType::CONJUGATE_GRADIENT;
        throw std::invalid_argument("Неверный тип алгоритма");
    }

    inline ExtremumType stringToExtremumType(const std::string& str) {
        if (str == "MINIMUM") return ExtremumType::MINIMUM;
        if (str == "MAXIMUM") return ExtremumType::MAXIMUM;
        throw std::invalid_argument("Неверный тип экстремума");
    }

    inline StepType stringToStepType(const std::string& str) {
        if (str == "CONSTANT")    return StepType::CONSTANT;
        if (str == "COEFFICIENT") return StepType::COEFFICIENT;
        if (str == "ADAPTIVE")    return StepType::ADAPTIVE;
        throw std::invalid_argument("Неверный тип шага");
    }
}
#endif // CONJUGATEGRADIENT_COMMON_HPP_
