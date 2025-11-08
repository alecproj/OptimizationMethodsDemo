//
// Created on 25 Oct, 2025
//  by alecproj
//

#ifndef COORDINATEDESCENT_COORDINATEDESCENT_HPP_
#define COORDINATEDESCENT_COORDINATEDESCENT_HPP_

#include <CoordinateDescent/Common.hpp>
#include <muParser.h>
#include <vector>
#include <functional>
#include <cmath>
#include <algorithm>

template <typename Reporter>
class CoordinateDescent {

    // Пример объявления и инициализации констант
    static constexpr double MIN_STEP{1e-10};       // Минимальный шаг (до 10^-10)
    static constexpr double STEP_REDUCTION{0.5};   // Коэффициент снижения шага
    static constexpr double MAX_STEP{1.0};         // Максимальный шаг (до 1.0)

public:

    CoordinateDescent(Reporter *reporter) :
        m_inputData{nullptr},
        m_reporter{reporter},
        m_parser{}, // Инициализация парсера muParser 
        m_x{0.0},
        m_y{0.0},
        m_function_calls{0},
        m_iterations{0}
    {
    }

    CDResult setInputData(const CoordinateInput *data)
    {
        if (!data) {
            return CDResult::InvalidInput; // проверка на nullptr
        }

        // ВАЛИДАЦИЯ ВХОДНЫХ ДАННЫХ

        // Проверка функции
        if (data->function.empty()) {
            return CDResult::EmptyFunction;
        }

        // Проверка синтаксиса функции
        CDResult syntax_check = validateFunctionSyntax(data->function);
        if (syntax_check != CDResult::Success) {
            return CDResult::ParseError;
        }

        // Проверка типа алгоритма
        if (data->algorithm_type != AlgorithmType::BASIC_COORDINATE_DESCENT &&
            data->algorithm_type != AlgorithmType::STEEPEST_COORDINATE_DESCENT) {
            return CDResult::InvalidAlgorithmType;
        }

        // Проверка типа экстремума
        if (data->extremum_type != ExtremumType::MINIMUM &&
            data->extremum_type != ExtremumType::MAXIMUM) {
            return CDResult::InvalidExtremumType;
        }

        // Проверка типа шага
        if (data->step_type != StepType::CONSTANT &&
            data->step_type != StepType::COEFFICIENT &&
            data->step_type != StepType::ADAPTIVE) {
            return CDResult::InvalidStepType;
        }

        // Проверка корректности границ X (вопрос про равенство)
        if ((data->x_left_bound >= data->x_right_bound) || (data->x_right_bound <= data-> x_left_bound)) {
            return CDResult::InvalidXBound;
        }
        

        // Проверка корректности границ Y (вопрос про равенство)
        if ((data->y_left_bound >= data->y_right_bound) || (data->y_right_bound <= data->y_left_bound)) {
            return CDResult::InvalidYBound;
        }

        // Проверка начального приближения X
        if (data->initial_x < data->x_left_bound || data->initial_x > data->x_right_bound) {
            return CDResult::InvalidInitialX;
        }

        // Проверка начального приближения Y
        if (data->initial_y < data->y_left_bound || data->initial_y > data->y_right_bound) {
            return CDResult::InvalidInitialY;
        }

        // Проверка точности результата
        if (data->result_precision <= 0.0 || data->result_precision > 1.0) {
            return CDResult::InvalidResultPrecision;
        }

        // Проверка точности вычислений
        if (data->computation_precision <= 0.0 || data->computation_precision > 1.0) {
            return CDResult::InvalidComputationPrecision;
        }

     /* // Проверка соотношения точностей (точность вычислений должно быть в 10 раз точнее результата)
        static constexpr double MIN_PRECISION_RATIO = 10.0;
        if (data->computation_precision * MIN_PRECISION_RATIO > data->result_precision) {
            return CDResult::InvalidInput;
        }*/

        // Проверка что точность вычислений меньше точности результата
        if (data->computation_precision > data->result_precision) {
            return CDResult::InvalidLogicPrecision;
        }

        // --- ВАЛИДАЦИЯ ПАРАМЕТРОВ ШАГА ---
        
        // Проверка постоянного шага
        if (data->constant_step_size <= 0.0) {
            return CDResult::InvalidConstantStepSize;
        }

        // Проверка коэффициента шага
        if (data->coefficient_step_size <= 0.0) {
            return CDResult::InvalidCoefficientStepSize;
        }

        // Сначала проверить все поля на корректность
        m_inputData = data;
        return CDResult::Success;
    }

    CDResult solve()
    {
        if (!m_inputData || !m_reporter || m_reporter->begin() == 0) {
            return CDResult::Fail;
        }

        CDResult result = CDResult::Success;

        try {
            // Инициализация Парсера
            initializeParser();

            // Выбор алгоритма
            switch (m_inputData->algorithm_type) {
                case AlgorithmType::BASIC_COORDINATE_DESCENT:
                    result = basicCoordinateDescent();
                    break;
                case AlgorithmType::STEEPEST_COORDINATE_DESCENT:
                    result = steepestCoordinateDescent();
                    break;
                default:
                    result = CDResult::InvalidAlgorithmType;
                    break;
            }
        } catch (const mu::Parser::exception_type& e) {
            result = CDResult::ParseError;
        } catch (const std::exception& e) {
            result = CDResult::ComputeError; // Может и поставить сюда Fail
        }

        // Процесс решения и записи отчета (разбить на приватные функции)

        if (m_reporter->end() == 0) {
            return CDResult::Success;
        }
        return CDResult::Fail;
    }

private:

    const CoordinateInput *m_inputData; // Настройки алгоритма
    Reporter* m_reporter; // Указатель на систему отчётности
    mu::Parser m_parser; // Система вычисления
    double m_x, m_y; // Текущие переменные для парсера
    int m_function_calls; // Счётчик вызовов функции
    int m_iterations; // Счётчик итераций

    // Инициализация парсера
    void initializeParser() {
        m_parser.SetExpr(m_inputData->function); // Загрузка заданной функции
        m_parser.DefineVar("x", &m_x); // Связывание переменной X с полем m_x
        m_parser.DefineVar("y", &m_y); // Связывание переменной Y с полем m_y
        m_function_calls = 0;
        m_iterations = 0;
    }

    // Провернка синтаксиса функции
    CDResult validateFunctionSyntax(const std::string& function) {
        try {
            mu::Parser test_parser;
            double test_x = 0.0;
            double test_y = 0.0;
            test_parser.SetExpr(m_inputData->function);
            test_parser.DefineVar("x", &test_x);
            test_parser.DefineVar("y", &test_y);

            // Пробуем вычислить в тестовой точке
            test_parser.Eval();

            return CDResult::Success;
        } catch (const mu::Parser::exception_type& e) {
            return CDResult::ParseError;
        } catch (...) {
            return CDResult::ParseError;
        }
    }

    // ============================================================================
    // ОСНОВНЫЕ ВСПОМОГАТЕЛЬНЫЕ МЕТОДЫ
    // ============================================================================

    // Вычисление функции в точке (x, y)
    double evaluateFunction(double x, double y) {
        m_x = x;
        m_y = y;
        m_function_calls++;
        try {
            return m_parser.Eval();
        } catch (...) {
            throw std::runtime_error("Ошибка вычисления функции в точке"); // отловить в solve()
        }
    }

    // Вычисление частной производной по X
    double partialDerivativeX(double x, double y) {
        double x_old = m_x, y_old = m_y;
        m_y = y; // Фиксируем y
        double derivative = m_parser.Diff(&m_x, x, m_inputData->computation_precision); // Вычисление частной производной по X из muParser
        m_x = x_old;
        m_y = y_old;
        return derivative;
    }
    
    // Вычисление частной производной по Y
    double partialDerivativeY(double x, double y) {
        double x_old = m_x, y_old = m_y;
        m_x = x; // Фиксируем x
        double derivative = m_parser.Diff(&m_y, y, m_inputData->computation_precision); // Вычисление частной производной по Y из muParser
        m_x = x_old;
        m_y = y_old;
        return derivative;
    }

// Проверка сходимости
bool checkConvergence(double x_old, double y_old,
    double x_new, double y_new,
    double f_old, double f_new) {

    // Вычисление изменений координат
    double dx = std::abs(x_new - x_old);
    double dy = std::abs(y_new - y_old);

    double df = std::abs(f_new - f_old);

    // Считаем евклидову норму вектора изменения координат
    double coordinate_norm = std::sqrt(dx * dx + dy * dy);
    return (coordinate_norm < m_inputData->result_precision &&
        df < m_inputData->result_precision);
}

// Проверка границ (на каждой итерации)
bool isWithinBounds(double x, double y) {
    return (x >= m_inputData->x_left_bound && x <= m_inputData->x_right_bound &&
        y >= m_inputData->y_left_bound && y <= m_inputData->y_right_bound);
}

// ============================================================================
// РЕАЛИЗАЦИИ ТИПОВ ШАГА
// ============================================================================

// Постоянный шаг
double getConstantStep() {
    // Если ищем минимум -> движемся вниз, максимум -> вверх
    double direction = (m_inputData->extremum_type == ExtremumType::MINIMUM) ? -1.0 : 1.0;
    return m_inputData->constant_step_size * direction;
}

// Коэффициентный шаг (зависимость от градиента)
double getCoefficientStep(double gradient) {
    // Если ищем минимум -> движемся вниз, максимум -> вверх
    double direction = (m_inputData->extremum_type == ExtremumType::MINIMUM) ? -1.0 : 1.0;
    return m_inputData->coefficient_step_size * gradient * direction;
}
// Адаптивный шаг
double getAdaptiveStep(double x, double y, double gradient, bool is_x) {
    // Если ищем минимум -> движемся вниз, максимум -> вверх
    double direction = (m_inputData->extremum_type == ExtremumType::MINIMUM) ? -1.0 : 1.0;
    double step = m_inputData->constant_step_size;
    double current_value = evaluateFunction(x, y);

    // Пробуем уменьшить шаг, пока не найдём улучшение
    for (int i = 0; i < 10; i++) {
        double x_new = is_x ? x + step * gradient * direction : x;
        double y_new = is_x ? y : y + step * gradient * direction;

        // Если вышли за границы, уменьшаем шаг вдвое
        if (!isWithinBounds(x_new, y_new)) {
            step *= STEP_REDUCTION;
            continue;
        }

        double new_value = evaluateFunction(x_new, y_new);
        // Проверка улучшения
        bool improvement = (m_inputData->extremum_type == ExtremumType::MINIMUM)
            ? (new_value < current_value)
            : (new_value > current_value);
        // нашли улучшение или функция плоская
        if (improvement || step < MIN_STEP) {
            break;
        }

        step *= STEP_REDUCTION;
    }
    return step * direction;
}

double getStepSize(double x, double y, double gradient, bool is_x) {
    switch (m_inputData->step_type) {
    case StepType::CONSTANT:
        return getConstantStep();
    case StepType::COEFFICIENT:
        return getCoefficientStep(gradient);
    case StepType::ADAPTIVE:
        return getAdaptiveStep(x, y, gradient, is_x);
    default:
        return getConstantStep();
    }
}

// ============================================================================
// АЛГОРИТМЫ ОПТИМИЗАЦИИ
// ============================================================================

 // Базовый алгоритм покоординатного спуска
CDResult basicCoordinateDescent() {
    double x = m_inputData->initial_x;
    double y = m_inputData->initial_y;
    double f_current = evaluateFunction(x, y);

    m_iterations = 0;

    while (m_iterations < m_inputData->max_iterations &&
           m_function_calls < m_inputData->max_function_calls) {
         
            double x_old = x, y_old = y;
            double f_old = f_current;

            // Оптимизация по X
            double grad_x = partialDerivativeX(x, y);
            double step_x = getStepSize(x, y, grad_x, true);
            x = updateCoordinate(x, step_x, m_inputData->x_left_bound, m_inputData->x_right_bound);

            // Оптимизация по Y
            double grad_y = partialDerivativeY(x, y);
            double step_y = getStepSize(x, y, grad_y, false);
            y = updateCoordinate(y, step_y, m_inputData->y_left_bound, m_inputData->y_right_bound);

            f_current = evaluateFunction(x, y);
            m_iterations++;

            // Отчёт о текущей итерации
           /* if (m_reporter) {
                m_reporter->reportIterations(iterations, x, y, f_current);
            }*/

            // Проверка сходимости
            if (checkConvergence(x_old, y_old, x, y, f_old, f_current)) {
                return CDResult::Success;
            }

            // Проверка границ
            if (!isWithinBounds(x, y)) {
                return CDResult::OutOfBounds;
            }
        }
        return checkTerminationCondition(m_iterations);

    }
    
    // Алгоритм наискорейшего покоординатного спуска
    CDResult steepestCoordinateDescent() {
        double x = m_inputData->initial_x;
        double y = m_inputData->initial_y;
        double f_current = evaluateFunction(x, y);

        m_iterations = 0;

        while (m_iterations < m_inputData->max_iterations &&
               m_function_calls < m_inputData->max_function_calls) {
            double x_old = x, y_old = y;
            double f_old = f_current;

            // Вычисление частных производных
            double grad_x = partialDerivativeX(x, y);
            double grad_y = partialDerivativeY(x, y);

            // Выбираем направление с наибольшей производной (наискорейший спуск)
            if (std::abs(grad_x) > std::abs(grad_y)) {
                // Движение по X
                double step_x = getStepSize(x, y, grad_x, true);
                x = updateCoordinate(x, step_x, m_inputData->x_left_bound, m_inputData->x_right_bound);
            } else {
                // Двигаемся по Y
                double step_y = getStepSize(x, y, grad_y, false);
                y = updateCoordinate(y, step_y, m_inputData->y_left_bound, m_inputData->y_right_bound);
            }

            f_current = evaluateFunction(x, y);
            m_iterations++;

            // Отчет
           /* if (m_reporter) {
                m_reporter->reportIteration(iterations, x, y, f_current);
            }*/

            // Проверка сходимости
            if (checkConvergence(x_old, y_old, x, y, f_old, f_current)) {
                return CDResult::Success;
            }

            // Проверка границ
            if (!isWithinBounds(x, y)) {
                return CDResult::OutOfBounds;
            }
        }
        return checkTerminationCondition(m_iterations);
    }

    // Обновление координаты с учётом границ
    double updateCoordinate(double coord, double step,
                                 double lower_bound, double upper_bound) {
        double new_coord = coord + step;
        return std::max(lower_bound, std::min(upper_bound, new_coord));
    }

    // Проверка условий завершения
    CDResult checkTerminationCondition(int iterations) {
        if (iterations >= m_inputData->max_iterations) {
            return CDResult::MaxIterations;
        }
        if (m_function_calls >= m_inputData->max_function_calls) {
            return CDResult::MaxFunctionsCalls;
        }
        return CDResult::Success;
    }
};



#endif // COORDINATEDESCENT_COORDINATEDESCENT_HPP_ 
