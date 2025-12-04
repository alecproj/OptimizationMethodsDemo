//
// Created on 25 Nov, 2025
//  by alecproj
//

#ifndef PARTICLESWARM_COMMON_HPP_
#define PARTICLESWARM_COMMON_HPP_

#include <string>

namespace PS {

// Результат выполнения алгоритма
enum class Result : int {
    Success = 0,                       // Успешное выполнение
    Fail = -1,                         // Общая ошибка
    InvalidInput = -2,                 // Неверные входные данные
    NoConvergence = -3,                // Нет сходимости
    ParseError = -7,                   // Ошибка парсинга функции
    ComputeError = -8,                 // Вычислительная ошибка
    NonDifferentiableFunction = -9,    // Функция не явл. дифференцируемой
    EmptyFunction = -10,               // Функция пустая
    InvalidExtremumType = -12,         // Неверный ввод типа экстремума
    InvalidXBound = -14,               // Неверный ввод границ X
    InvalidYBound = -15,               // Неверный ввод границ Y
    InvalidResultPrecision = -18,      // Неверный ввод точности результата
    InvalidComputationPrecision = -19, // Неверный ввод точности вычислений
    InvalidLogicPrecision = -20,       // Неверный ввод точностей
};

enum class ExtremumType {
    MINIMUM,
    MAXIMUM
};

struct InputData {

    std::string function;           // Функция для оптимизации
    ExtremumType extremum_type;     // Тип экстремума

    // --- ГРАНИЦЫ ПОИСКА ---
    double x_left_bound;            // Левая граница диапазона X
    double x_right_bound;           // Правая граница диапазона X
    double y_left_bound;            // Левая граница диапазона Y
    double y_right_bound;           // Правая граница диапазона Y

    // --- ПАРАМЕТРЫ ТОЧНОСТИ ---
    int result_precision;           // Точность результата
    int computation_precision;      // Точность вычислений
};

inline std::string resultToString(Result result) {
    switch (result) {
        case Result::Success:                     return "Успешно";
        case Result::Fail:                        return "Произошла ошибка";
        case Result::InvalidInput:                return "Некорректные входные данные";
        case Result::NoConvergence:               return "Алгоритм не сходится";
        case Result::ParseError:                  return "Ошибка обработки функции";
        case Result::ComputeError:                return "Вычислительная ошибка";
        case Result::NonDifferentiableFunction:   return "Функция не является дифференцируемой";
        case Result::EmptyFunction:               return "Функция не передана";
        case Result::InvalidExtremumType:         return "Неверный ввод типа экстремума";
        case Result::InvalidXBound:               return "Неверный ввод границ X";
        case Result::InvalidYBound:               return "Неверный ввод границ Y";
        case Result::InvalidResultPrecision:      return "Неверный ввод точности результата";
        case Result::InvalidComputationPrecision: return "Неверный ввод точности вычислений";
        case Result::InvalidLogicPrecision:       return "Неверный ввод точностей";
        default:                                  return "Unknown result";
    }
}

} // namespace GA
#endif // PARTICLESWARM_COMMON_HPP_
