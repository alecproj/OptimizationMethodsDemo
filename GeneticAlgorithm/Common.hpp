//
// Created on 25 Nov, 2025
//  by alecproj
//

#ifndef GENETICALGORITHM_COMMON_HPP_
#define GENETICALGORITHM_COMMON_HPP_

#include <string>
#include <vector>
#include <cstdint>
#include <cstddef>
#include <stdexcept>

namespace GA {

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

struct Individual {
    double x, y;
    std::vector<bool> chromosome; // Вектор булевых значений (кодирование точки)
    double fitness;               // Значение приспособленности (f(x,y))

    // Конструктор с параметрами по умолчанию
    Individual(double x_val = 0.0, double y_val = 0.0, double fit = 0.0)
        :x(x_val), y(y_val), fitness(fit) {}
};

struct GAConfig {
    size_t   population_size;   // Количество особей в популяции (сколько решений (точек) одновременно обрабатывается)
    size_t   generations;       // Количество поколений (Сколько раз обновляем популяцию-максимальное количество итераций алгоритма)
    double   crossover_rate;    // Вероятность кроссовера (Вероятность скрещивания двух родительских особей)
    double   mutation_rate;     // Вероятность мутации (Вероятность изменения случайного бита в хромосоме)
    uint64_t bits_per_variable; // Точность представления чисел в двоичном коде
    size_t   elite_count;       // Количество элитных особей (Количество лучших особей, которые переходят в следующее поколение без изменений)

    // Конструктор с параметрами по умолчанию
    GAConfig(size_t pop_size = 50, size_t gens = 100, 
             double cross_rate = 0.8, double mut_rate = 0.01,
             uint64_t bits = 32, size_t elite = 2)
        : population_size(pop_size), generations(gens),
          crossover_rate(cross_rate), mutation_rate(mut_rate),
          bits_per_variable(bits), elite_count(elite) 
    {
        if (bits > 52) {
            throw std::invalid_argument("Количество бит не может превышать 52");
        }
    }
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
#endif // GENETICALGORITHM_COMMON_HPP_
