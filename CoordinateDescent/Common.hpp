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
    Success            = 0,            // Успешное выполнение
    Fail              = -1,            // Общая ошибка
    InvalidInput      = -2,            // Неверные входные данные
    NoConvergence     = -3,            // Нет сходимости
    OutOfBounds       = -4,            // Выход за границы
    MaxIterations     = -5,            // Превышение макс. числа итераций
    MaxFunctionsCalls = -6,            // Превышение макс. числа вызовов функции
    ParseError        = -7,            // Ошибка парсинга функции
    ComputeError      = -8,            // Вычислительная ошибка
    NonDifferentiable = -9,            // Функция не явл. дифференцируемой
    EmptyFunction     = -10,           // Функция пустая
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
double gradient_epsilon = 1e-8;     // Точность вычисления градиента

// Основные входные параметры для алгоритма
struct CoordinateInput {

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
struct CoordinateResult {
    CDResult status = CDResult::Success;    // Статус выполнения
    std::vector<double> optimum_point; // Точка экстремума [X, Y]
    double optimum_value;              // Значение функции в точке экстремума
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
        case CDResult::Success:                     return "Ошибка: Успешно";
        case CDResult::Fail:                        return "Ошибка: Произошла ошибка";
        case CDResult::InvalidInput:                return "Ошибка: Некорректные входные данные";
        case CDResult::NoConvergence:               return "Ошибка: Алгоритм не сходится";
        case CDResult::OutOfBounds:                 return "Ошибка: Выход за границы";
        case CDResult::MaxIterations:               return "Ошибка: Достигнут максимум итераций";
        case CDResult::MaxFunctionsCalls:           return "Ошибка: Достигнут максимум вызовов функции";
        case CDResult::ParseError:                  return "Ошибка: Ошибка обработки функции";
        case CDResult::ComputeError:                return "Ошибка: Вычислительная ошибка";
        case CDResult::NonDifferentiable:           return "Ошибка: Функция не является дифференцируемой";
        case CDResult::EmptyFunction:               return "Ошибка: Функция простая";
        case CDResult::InvalidAlgorithmType:        return "Ошибка: Неверный ввод типа алгоритма";
        case CDResult::InvalidExtremumType:         return "Ошибка: Неверный ввод типа экстремума";
        case CDResult::InvalidStepType:             return "Ошибка: Неверный ввод типа шага";
        case CDResult::InvalidXBound:               return "Ошибка: Неверный ввод границ X";
        case CDResult::InvalidYBound:               return "Ошибка: Неверный ввод границ Y";
        case CDResult::InvalidInitialX:             return "Ошибка: Неверный ввод начального приближения X";
        case CDResult::InvalidInitialY:             return "Ошибка: Неверный ввод начального приближения Y";
        case CDResult::InvalidResultPrecision:      return "Ошибка: Неверный ввод точности результата";
        case CDResult::InvalidComputationPrecision: return "Ошибка: Неверный ввод точности вычислений"; 
        case CDResult::InvalidLogicPrecision:       return "Ошибка: Неверный ввод точностей";
        case CDResult::InvalidConstantStepSize:     return "Ошибка: Неверный ввод постоянного шага";
        case CDResult::InvalidCoefficientStepSize:  return "Ошибка: Неверный ввод коэффициентного шага";
        default:                                    return "Unknown result";
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


#endif // COORDINATEDESCENT_COMMON_HPP_