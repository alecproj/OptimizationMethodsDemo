//
// Created on 25 Oct, 2025
//  by alecproj
//

#ifndef COORDINATEDESCENT_COMMON_HPP_
#define COORDINATEDESCENT_COMMON_HPP_

#include <string>
#include <vector>


// ============================================================================
// Перечисления состояний и ошибок
// ============================================================================

// Результат выполнения алгоритма
enum class CDResult : int {
    Success            = 0,   // Успешное выполнение
    Fail              = -1,   // Общая ошибка
    InvalidInput      = -2,   // Неверные входные данные
    NoConvergence     = -3,   // Нет сходимости
    OutOfBounds       = -4,   // Выход за границы
    MaxIterations     = -5,   // Превышение макс. числа итераций
    MaxFunctionsCalls = -6,   // Превышение макс. числа вызовов функции
    ParseError        = -7,   // Ошибка парсинга функции
    ComputeError      = -8,   // Вычислительная ошибка
    NonDifferentiable = -9    // Функция не явл. дифференцируемой
};

// Тип алгоритма оптимизации
enum class AlgorithmType {
    BASIC_COORDINATE_DESCENT,    // Базовый алгоритм покоординатного спуска
    STEEPEST_COORDINATE_DESCENT, // Расширение: покоординатный спуск методом наискорейшего спуска
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

// Состояние выполнения алгоритма

enum class AlgorithmState {
    NOT_STARTED,  // Алгоритм не запущен
    INITIALIZING, // Инициализация алгоритма
    RUNNING ,      // Алгоритм выполняется
    CONVERGED,    // Алгоритм сходится
    STOPPED,      // Алгоритм завершил работу успешно
    FAILED        // Алгоритм завершил работу с ошибкой
};

// ============================================================================
// Структуры входных данных
// ============================================================================
long double gradient_epsilon = 1e-8;     // Точность вычисления градиента

// Основные входные параметры для алгоритма
struct CoordinateInput {

    // --- ОБЯЗАТЕЛЬНЫЕ ПАРАМЕТРЫ ---
    std::string function;         // Функция для оптимизации
    AlgorithmType algorithm_type; // Тип алгоритма
    ExtremumType extremum_type;   // Тип экстремума

    // --- НАЧАЛЬНЫЕ УСЛОВИЯ ---
    long double initial_x = 0.0; // Начальное приближение X
    long double initial_y = 0.0; // Начальное приближение Y

    // --- ГРАНИЦЫ ПОИСКА ---
    long double x_left_bound = -10.0; // Левая граница диапазона X
    long double x_right_bound = 10.0; // Правая граница диапазона X
    long double y_left_bound = -10.0; // Левая граница диапазона Y
    long double y_right_bound = 10.0; // Правая граница диапазона Y

    // --- ПАРАМЕТРЫ ТОЧНОСТИ ---
    long double result_precision = 1e-06;     // Точность результата
    long double computation_precision = 1e-8; // Точность вычислений
     
    // --- ОГРАНИЧЕНИЯ ---
    int max_iterations = 1000;       // Макс. число итераций 
    int max_function_calls = 10000;  // Макс. число вызовов функции

};

// ============================================================================
// Структура выходных данных
// ============================================================================

// Результат оптимизации
struct CoordinateResult {
    CDResult status = CDResult::Success;    // Статус выполнения
    std::vector<long double> optimum_point; // Точка экстремума [X, Y]
    long double optimum_value;              // Значение функции в точке экстремума
    int iterations = 0;                     // Кол-во выполненных итераций
    int function_calls = 0;                 // Кол-во вызовов функции
    std::string message;                    // Сообщение о результате
};

// ============================================================================
// Вспомогательные функции
// ============================================================================

// Конвертация результата алгоритма в строковое сообщение
inline std::string resultToString(CDResult result) {
    switch (result) {
        case CDResult::Success:           return "Успешно";
        case CDResult::Fail:              return "Произошла ошибка";
        case CDResult::InvalidInput:      return "Некорректные входные данные";
        case CDResult::NoConvergence:     return "Алгоритм не сходится";
        case CDResult::OutOfBounds:       return "Выход за границы";
        case CDResult::MaxIterations:     return "Достигнут максимум итераций";
        case CDResult::MaxFunctionsCalls: return "Достигнут максимум вызовов функции";
        case CDResult::ParseError:        return "Ошибка обработки функции";
        case CDResult::ComputeError:      return "Вычислительная ошибка";
        case CDResult::NonDifferentiable: return "Функция не является дифференцируемой";
        
        default:                          return "Unknown result";
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


#endif // COORDINATEDESCENT_COMMON_HPP_