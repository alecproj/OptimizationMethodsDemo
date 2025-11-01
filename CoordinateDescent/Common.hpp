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
    DERIVATIVE_FREE_DESCENT      // Покоординатный спуск (без производных)
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

// Параметры для алгоритма базового покоординатного спуска
struct BasicAlgorithmParams {
    long double x_step  = 0.1;               // Шаг по X
    long double y_step = 0.1;                // Шаг по Y
    long double coefficient_step = 1.0;      // Коэффициентный шаг
    StepType step_type = StepType::CONSTANT; // Тип шага
    long double gradient_epsilon = 1e-8;     // Точность вычисления градиента
};

// Параметры для алгоритма покоординатного спуска методом наискорейшего спуска
struct SteepestAlgorithmParams {

    long double learning_rate = 0.01;         // Скорость "обучения"
    long double gradient_epsilon = 1e-8;      // Точность вычисления градиента
    long double line_search_precision = 1e-6; // Точность линейного поиска

};

// Параметры для беспроизводного покоординатного спуска
struct DerivativeFreeAlgorithmParams {
    long double initial_step = 0.1;      // Начальный шаг
    long double step_reduction = 0.5;    // Коэффициент уменьшения шага
    long double exploration_step = 0.01; // Шаг для исследования
    int max_exploration_attempts = 10;   // Макс. попыток исследования
};

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
    
    // --- АЛГОРИТМО-СПЕЦИФИЧНЫЕ ПАРАМЕТРЫ ---
    BasicAlgorithmParams basic_params;                    // Для BASIC_COORDINATE_DESCENT
    SteepestAlgorithmParams steepest_params;              // Для STEEPEST_COORDINATE_DESCENT
    DerivativeFreeAlgorithmParams derivative_free_params; // Для DERIVATIVE_FREE_DESCENT      

    // --- ОГРАНИЧЕНИЯ ---
    int max_iterations = 1000;       // Макс. число итераций 
    int max_function_calls = 10000;  // Макс. число вызовов функции

    // --- ДОПОЛНИТЕЛЬНЫЕ ФЛАГИ ---
    bool enable_logging = false;         // Включение логирования
    bool save_iteration_history = false; // Сохранение истории итераций

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
        case CDResult::Success:           return "Success";
        case CDResult::Fail:              return "General failure";
        case CDResult::InvalidInput:      return "Invalid input parameters";
        case CDResult::NoConvergence:     return "No convergence";
        case CDResult::OutOfBounds:       return "Point out of bounds";
        case CDResult::MaxIterations:     return "Maximum iterations exceeded";
        case CDResult::MaxFunctionsCalls: return "Maximum function calls exceeded";
        case CDResult::ParseError:        return "Function parsing error";
        case CDResult::ComputeError:      return "Computation error";
        case CDResult::NonDifferentiable: return "Function is not differentiable";
        
        default:                          return "Unknown result";
    }
}

// Конвертация типа алгоритма в строковое сообщение
inline std::string algorithmTypeToString(AlgorithmType type) {
    switch (type) {
        case AlgorithmType::BASIC_COORDINATE_DESCENT:    return "Basic Coordinate Descent";
        case AlgorithmType::STEEPEST_COORDINATE_DESCENT: return "Steepest Coordinate Descent";
        case AlgorithmType::DERIVATIVE_FREE_DESCENT:     return "Derivative-Free Coordinate Descent";
        
        default:                                         return "Unknown Algorithm";
    }
}

// Конвертация типа экстремума в строку
inline std::string extremumTypeToString(ExtremumType type) {
    switch (type) {
        case ExtremumType::MINIMUM: return "Minimum";
        case ExtremumType::MAXIMUM: return "Maximum";

        default:                    return "Unknow";
    }
}

// Проверка: требуется ли вычислять производную функции
inline bool algorithmRequiresDerivatives(AlgorithmType type) {
    return type == AlgorithmType::BASIC_COORDINATE_DESCENT ||
        type == AlgorithmType::STEEPEST_COORDINATE_DESCENT;
}

#endif // COORDINATEDESCENT_COMMON_HPP_