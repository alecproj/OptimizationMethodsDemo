//
// Created on 25 Oct, 2025
//  by alecproj
//

#ifndef COORDINATEDESCENT_COORDINATEDESCENT_HPP_
#define COORDINATEDESCENT_COORDINATEDESCENT_HPP_

#include <CoordinateDescent/Common.hpp>
#include <muParser.h>
#include <vector>
#include <cmath>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace CD {
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

    double getX() const          { return m_x; }                        // Получить X
    double getY() const          { return m_y; }                        // Получить Y
    int getIterations() const    { return m_iterations; }               // Получить кол-во итераций
    int getFunctionCalls() const { return m_function_calls; }           // Получить кол-во вызовов функции
    double getOptimumValue()     { return evaluateFunction(m_x, m_y); } // Вычисление значение функции в финальной точке


    Result setInputData(const InputData *data)
    {
        if (!data) {
            return Result::InvalidInput; // проверка на nullptr
        }

        // ВАЛИДАЦИЯ ВХОДНЫХ ДАННЫХ
        // Проверка функции
        if (data->function.empty()) {
            return Result::EmptyFunction;
        }

        // Проверка синтаксиса функции
        Result syntax_check = validateFunctionSyntax(data->function);
        if (syntax_check != Result::Success) {
            return Result::ParseError;
        }

        // Проверка на дифференцируемость
        Result differentiability_check = checkFunctionDifferentiability(data->function);
        if (differentiability_check != Result::Success) {
            return differentiability_check;
        }

        // Проверка типа алгоритма
        if (data->algorithm_type != AlgorithmType::BASIC_COORDINATE_DESCENT &&
            data->algorithm_type != AlgorithmType::STEEPEST_COORDINATE_DESCENT) {
            return Result::InvalidAlgorithmType;
        }

        // Проверка типа экстремума
        if (data->extremum_type != ExtremumType::MINIMUM &&
            data->extremum_type != ExtremumType::MAXIMUM) {
            return Result::InvalidExtremumType;
        }

        // Проверка типа шага
        if (data->step_type != StepType::CONSTANT &&
            data->step_type != StepType::COEFFICIENT &&
            data->step_type != StepType::ADAPTIVE) {
            return Result::InvalidStepType;
        }

        // Проверка типа шага X
        if (data->step_type_x != StepType::CONSTANT &&
            data->step_type_x != StepType::COEFFICIENT &&
            data->step_type_x != StepType::ADAPTIVE) {
            return Result::InvalidStepTypeX;
        }

        // Проверка типа шага Y
        if (data->step_type_y != StepType::CONSTANT &&
            data->step_type_y != StepType::COEFFICIENT &&
            data->step_type_y != StepType::ADAPTIVE) {
            return Result::InvalidStepTypeY;
        }

        // Проверка корректности границ X (вопрос про равенство)
        if ((data->x_left_bound >= data->x_right_bound)) {
            return Result::InvalidXBound;
        }
        

        // Проверка корректности границ Y (вопрос про равенство)
        if ((data->y_left_bound >= data->y_right_bound)) {
            return Result::InvalidYBound;
        }

        // Проверка начального приближения X
        if (data->initial_x < data->x_left_bound || data->initial_x > data->x_right_bound) {
            return Result::InvalidInitialX;
        }

        // Проверка начального приближения Y
        if (data->initial_y < data->y_left_bound || data->initial_y > data->y_right_bound) {
            return Result::InvalidInitialY;
        }

        // Проверка точности результата
        if (data->result_precision <= 0.0 || data->result_precision > 1.0) {
            return Result::InvalidResultPrecision;
        }

        // Проверка точности вычислений
        if (data->computation_precision <= 0.0 || data->computation_precision > 1.0) {
            return Result::InvalidComputationPrecision;
        }

     /* // Проверка соотношения точностей (точность вычислений должно быть в 10 раз точнее результата)
        static constexpr double MIN_PRECISION_RATIO = 10.0;
        if (data->computation_precision * MIN_PRECISION_RATIO > data->result_precision) {
            return CDResult::InvalidInput;
        }*/

        // Проверка что точность вычислений меньше точности результата
        if (data->computation_precision > data->result_precision) {
            return Result::InvalidLogicPrecision;
        }

        // --- ВАЛИДАЦИЯ ПАРАМЕТРОВ ШАГА ---
        
        // Проверка постоянного шага
        if (data->constant_step_size <= 0.0) {
            return Result::InvalidConstantStepSize;
        }

        // Проверка коэффициента шага
        if (data->coefficient_step_size <= 0.0) {
            return Result::InvalidCoefficientStepSize;
        }

        // Проверка типов шагов для X
        if (data->constant_step_size_x <= 0.0) {
            return Result::InvalidConstantStepSizeX;
        }
        if (data->coefficient_step_size_x <= 0.0) {
            return Result::InvalidCoefficientStepSizeX;
        }

        // Проверка типов шагов для Y
        if (data->constant_step_size_y <= 0.0) {
            Result::InvalidConstantStepSizeY;
        }
        if (data->coefficient_step_size_y <= 0.0) {
            Result::InvalidCoefficientStepSizeY;
        }

        // Сначала проверить все поля на корректность
        m_inputData = data;
        return Result::Success;
    }

    Result solve()
    {
        if (!m_inputData || !m_reporter || m_reporter->begin() != 0) {
            return Result::Fail;
        }

        Result result = Result::Success;

        try {
            // Инициализация Парсера
            initializeParser();

            if (!isFunctionDifferentiableAtStart()) {
                return Result::NonDifferentiableFunction;
            }

            // Выбор алгоритма
            switch (m_inputData->algorithm_type) {
                case AlgorithmType::BASIC_COORDINATE_DESCENT:
                    result = basicCoordinateDescent();
                    break;
                case AlgorithmType::STEEPEST_COORDINATE_DESCENT:
                    result = steepestCoordinateDescent();
                    break;
                default:
                    result = Result::InvalidAlgorithmType;
                    break;
            }
        } catch (const mu::Parser::exception_type& e) {
            result = Result::ParseError;
        } catch (const std::exception& e) {
            result = Result::ComputeError; // Может и поставить сюда Fail
        }

        // Процесс решения и записи отчета (разбить на приватные функции)

        if (m_reporter->end() == 0) {
            return Result::Success;
        }
        return Result::Fail;
    }

private:

    const InputData *m_inputData; // Настройки алгоритма
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
    Result validateFunctionSyntax(const std::string& function) {
        try {
            mu::Parser test_parser;
            double test_x = 0.0;
            double test_y = 0.0;
            test_parser.SetExpr(function);
            test_parser.DefineVar("x", &test_x);
            test_parser.DefineVar("y", &test_y);

            // Пробуем вычислить в тестовой точке
            test_parser.Eval();

            return Result::Success;
        } catch (const mu::Parser::exception_type& e) {
            return Result::ParseError;
        } catch (...) {
            return Result::ParseError;
        }
    }

    // Проверка дифференцируемости в начальной точке
    bool isFunctionDifferentiableAtStart() {
        try {
            double x = m_inputData->initial_x;
            double y = m_inputData->initial_y;

            // Проверяем производные в начальной точке
            partialDerivativeX(x, y);
            partialDerivativeY(x, y);

            return true;
        }
        catch (...) {
            std::cout << "Функция не дифференцируема в начальной точке ("
                << m_inputData->initial_x << ", " << m_inputData->initial_y << ")" << std::endl;
            return false;
        }
    }

    Result checkFunctionDifferentiability(const std::string& function) {
        try {
            
            // Предварительная проверка на известные недифференцируемые функции
            std::vector<std::string> non_diff_functions = {
                "abs(", "|", "sign(", "floor(", "ceil(", "round(",
                "fmod(", "mod(", "rand(", "max(", "min(", "random"
            };

            std::string func_lower = function;
            std::transform(func_lower.begin(), func_lower.end(), func_lower.begin(), ::tolower);

            for (const auto& non_diff_func : non_diff_functions) {
                std::string non_diff_lower = non_diff_func;
                std::transform(non_diff_lower.begin(), non_diff_lower.end(), non_diff_lower.begin(), ::tolower);

                if (func_lower.find(non_diff_lower) != std::string::npos) {
                    std::cout << "Обнаружена потенциально недифференцируемая функция: " << non_diff_func << std::endl;
                    std::cout << "Функция содержит: " << function << std::endl;
                    return Result::NonDifferentiableFunction;
                }
            } 
            
            // Проверка численной дифференцируемости
            mu::Parser test_parser;
            double test_x = 0.0;
            double test_y = 0.0;
            test_parser.SetExpr(function);
            test_parser.DefineVar("x", &test_x);
            test_parser.DefineVar("y", &test_y);

            const int TEST_POINTS = 8;
            std::vector<std::pair<double, double>> test_points;

            double center_x = m_inputData ? m_inputData->initial_x : 0.0;
            double center_y = m_inputData ? m_inputData->initial_y : 0.0;

            for (int i = 0; i < TEST_POINTS; i++) {
                double angle = 2.0 * M_PI * i / TEST_POINTS;
                double radius = 0.1;
                test_points.push_back({
                    center_x + radius * std::cos(angle),
                    center_y + radius * std::sin(angle)
                    });
            }

            for (const auto& point : test_points) {
                test_x = point.first;
                test_y = point.second;

                try {
                    double func_value = test_parser.Eval();
                    // Проверяем производные с разной точностью
                    double deriv_x1 = 0.0, deriv_x2 = 0.0;
                    double deriv_y1 = 0.0, deriv_y2 = 0.0;

                    // Первая попытка с обычной точностью
                    deriv_x1 = test_parser.Diff(&test_x, test_x, 1e-6);
                    deriv_y1 = test_parser.Diff(&test_y, test_y, 1e-6);

                    // Вторая попытка с другой точностью для проверки стабильности
                    deriv_x2 = test_parser.Diff(&test_x, test_x, 1e-7);
                    deriv_y2 = test_parser.Diff(&test_y, test_y, 1e-7);

                    // Проверяем, что производные не "взрываются"
                    if (std::isnan(deriv_x1) || std::isinf(deriv_x1) ||
                        std::isnan(deriv_y1) || std::isinf(deriv_y1) ||
                        std::isnan(deriv_x2) || std::isinf(deriv_x2) ||
                        std::isnan(deriv_y2) || std::isinf(deriv_y2)) {
                        std::cout << "Производная не определена в точке ("
                            << test_x << ", " << test_y << ")" << std::endl;
                        return Result::NonDifferentiableFunction;
                    }
                } catch (const mu::Parser::exception_type& e) {
                    std::cout << "Функция не дифференцируема в точке ("
                        << test_x << ", " << test_y << "): " << e.GetMsg() << std::endl;
                    return Result::NonDifferentiableFunction;
                } catch (const std::exception& e) {
                    std::cout << "Ошибка дифференцирования в точке ("
                        << test_x << ", " << test_y << "): " << e.what() << std::endl;
                    return Result::NonDifferentiableFunction;
                }
            }
            std::cout << "Функция прошла проверку дифференцируемости" << std::endl;
            return Result::Success;

        } catch (const mu::Parser::exception_type& e) {
            std::cout << "Ошибка парсера при проверке дифференцируемости: " << e.GetMsg() << std::endl;
            return Result::ParseError;
        } catch (const std::exception& e) {
            std::cout << "Общая ошибка при проверке дифференцируемости: " << e.what() << std::endl;
            return Result::ComputeError;
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
        double f_old, double f_new,
        double& best_x, double& best_y, double& best_f) { // передаем по ссылке!

        double dx = std::abs(x_new - x_old);
        double dy = std::abs(y_new - y_old);
        double df = std::abs(f_new - f_old);

        double coordinate_norm = std::sqrt(dx * dx + dy * dy);

        // УЛУЧШЕННЫЙ ДЕТЕКТОР ОСЦИЛЛЯЦИЙ
        static std::vector<std::pair<double, double>> recent_points;
        static int oscillation_count = 0;

        // Сохраняем последние 5 точек
        recent_points.push_back({ x_new, y_new });
        if (recent_points.size() > 5) {
            recent_points.erase(recent_points.begin());
        }

        // Проверяем все возможные циклы в последних точках
        if (recent_points.size() >= 4) {
            bool found_cycle = false;
            for (size_t i = 0; i < recent_points.size() - 2; ++i) {
                for (size_t j = i + 1; j < recent_points.size() - 1; ++j) {
                    double dist = std::sqrt(
                        std::pow(recent_points[i].first - recent_points[j].first, 2) +
                        std::pow(recent_points[i].second - recent_points[j].second, 2)
                    );
                    if (dist < m_inputData->computation_precision) {
                        oscillation_count++;
                        found_cycle = true;
                        break;
                    }
                }
                if (found_cycle) break;
            }

            if (oscillation_count > 3) { // уменьшил порог для более раннего обнаружения
                std::cout << "*** STOP: Oscillation detected after "
                    << oscillation_count << " cycles ***" << std::endl;

                // ПРИНУДИТЕЛЬНО УСТАНАВЛИВАЕМ ЛУЧШУЮ ТОЧКУ
                if (m_inputData->extremum_type == ExtremumType::MAXIMUM) {
                    // Для максимума ищем точку с наибольшим значением функции
                    double max_f = best_f;
                    for (const auto& point : recent_points) {
                        double f_val = evaluateFunction(point.first, point.second);
                        if (f_val > max_f) {
                            max_f = f_val;
                            best_x = point.first;
                            best_y = point.second;
                            best_f = f_val;
                        }
                    }
                }
                else {
                    // Для минимума ищем точку с наименьшим значением функции
                    double min_f = best_f;
                    for (const auto& point : recent_points) {
                        double f_val = evaluateFunction(point.first, point.second);
                        if (f_val < min_f) {
                            min_f = f_val;
                            best_x = point.first;
                            best_y = point.second;
                            best_f = f_val;
                        }
                    }
                }
                return true;
            }

            if (!found_cycle) {
                oscillation_count = 0; // сбрасываем счетчик если цикл прервался
            }
        }

        // ОСНОВНОЙ КРИТЕРИЙ СХОДИМОСТИ - с учетом лучшей точки
        if (coordinate_norm < m_inputData->result_precision &&
            df < m_inputData->result_precision) {

            // ПЕРЕД ВОЗВРАТОМ УБЕДИТЕСЬ, ЧТО ИСПОЛЬЗУЕМ ЛУЧШУЮ ТОЧКУ
            double current_f = evaluateFunction(x_new, y_new);
            if ((m_inputData->extremum_type == ExtremumType::MAXIMUM && current_f > best_f) ||
                (m_inputData->extremum_type == ExtremumType::MINIMUM && current_f < best_f)) {
                best_x = x_new;
                best_y = y_new;
                best_f = current_f;
            }

            std::cout << "*** CONVERGENCE: Coordinates and function stabilized ***" << std::endl;
            return true;
        }

        return false;
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
    double getConstantStep(double gradient, bool is_x) {
        // Если ищем минимум -> движемся вниз, максимум -> вверх
        double direction = (m_inputData->extremum_type == ExtremumType::MINIMUM) ? -1.0 : 1.0;
        double sign_gradient = (gradient >= 0) ? 1.0 : -1.0;

        double constant_step_size = is_x ? m_inputData->constant_step_size_x
                                : m_inputData->constant_step_size_y;
        return constant_step_size * direction * sign_gradient;
    }

    // Коэффициентный шаг (зависимость от градиента)
    double getCoefficientStep(double gradient, bool is_x) {
        // Если ищем минимум -> движемся вниз, максимум -> вверх
        double direction = (m_inputData->extremum_type == ExtremumType::MINIMUM) ? -1.0 : 1.0;
        double coefficient_step_size = is_x ? m_inputData->coefficient_step_size_x
                                            : m_inputData->coefficient_step_size_y;
        return coefficient_step_size * gradient * direction;
    }
    // Адаптивный шаг
    double getAdaptiveStep(double x, double y, double gradient, bool is_x) {
        // Если ищем минимум -> движемся вниз, максимум -> вверх
        double direction = (m_inputData->extremum_type == ExtremumType::MINIMUM) ? -1.0 : 1.0;
       
        double initial_step = is_x ? m_inputData->constant_step_size_x
                                   : m_inputData->constant_step_size_y;
        
        double step = initial_step;
        double current_value = evaluateFunction(x, y);
        double best_delta = 0.0;
        double best_value = current_value;

        // Пробуем уменьшить шаг, пока не найдём улучшение
        for (int i = 0; i < 15; i++) {
            double delta = step * direction * gradient;
            double x_new = is_x ? x + delta : x;
            double y_new = is_x ? y : y + delta;

            if (!isWithinBounds(x_new, y_new)) {
                step *= STEP_REDUCTION;
                continue;
            }

            double new_value = evaluateFunction(x_new, y_new);
            // Проверка улучшения
            bool improvement = (m_inputData->extremum_type == ExtremumType::MINIMUM)
                ? (new_value < best_value)
                : (new_value > best_value);

            if (improvement) {
                best_value = new_value;
                best_delta = delta;
            }
            step *= STEP_REDUCTION;
            if (step < MIN_STEP) break;
        }
        return best_delta;
    }

    double getStepSize(double x, double y, double gradient, bool is_x) {
        
        StepType current_step_type = is_x ? m_inputData->step_type_x
                                          : m_inputData->step_type_y;
        
        switch (current_step_type) {
        case StepType::CONSTANT:
            return getConstantStep(gradient, is_x);
        case StepType::COEFFICIENT:
            return getCoefficientStep(gradient, is_x);
        case StepType::ADAPTIVE:
            return getAdaptiveStep(x, y, gradient, is_x);
        default:
            return getConstantStep(gradient, is_x);
        }
    }

// ============================================================================
// АЛГОРИТМЫ ОПТИМИЗАЦИИ
// ============================================================================

 // Базовый алгоритм покоординатного спуска
Result basicCoordinateDescent() {
    double x = m_inputData->initial_x;
    double y = m_inputData->initial_y;
    double f_current = evaluateFunction(x, y);

    double best_x = x, best_y = y, best_f = f_current;
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

            bool improvement = (m_inputData->extremum_type == ExtremumType::MINIMUM)
                ? (f_current < best_f)
                : (f_current > best_f);

            if (improvement) {
                best_x = x;
                best_y = y;
                best_f = f_current;
            }

            // Отчёт о текущей итерации
           /* if (m_reporter) {
                m_reporter->reportIterations(m_iterations, best_x, best_y, best_f);
            }*/

            // Проверка сходимости
            // Проверка сходимости с передачей лучших значений
            if (checkConvergence(x_old, y_old, x, y, f_old, f_current,
                                 best_x, best_y, best_f)) {
                m_x = best_x;
                m_y = best_y;
                f_current = best_f;
                return Result::Success;
            }

            // Проверка границ
            if (!isWithinBounds(x, y)) {
                m_x = best_x;
                m_y = best_y;
                return Result::OutOfBounds;
            }
            std::cout << "Итерация " << m_iterations
                << ": x=" << x << ", y=" << y
                << ", f=" << f_current << std::endl
                << ", ЛУЧШАЯ f=" << best_f << std::endl;
        }

        m_x = best_x;
        m_y = best_y;
        f_current = best_f;
        return checkTerminationCondition();

    }
    
    // Алгоритм наискорейшего покоординатного спуска
    Result steepestCoordinateDescent() {
        double x = m_inputData->initial_x;
        double y = m_inputData->initial_y;
        double f_current = evaluateFunction(x, y);

        double best_x = x, best_y = y, best_f = f_current;
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

            bool improvement = (m_inputData->extremum_type == ExtremumType::MINIMUM)
                ? (f_current < best_f)
                : (f_current > best_f);
            
            if (improvement) {
                best_x = x;
                best_y = y;
                best_f = f_current;
            }

            // Отчет
           /* if (m_reporter) {
                m_reporter->reportIteration(iterations, x, y, f_current);
            }*/

            // Проверка сходимости
            if (checkConvergence(x_old, y_old, x, y, f_old, f_current,
                                 best_x, best_y, best_f)) {
                m_x = best_x;
                m_y = best_y;
                f_current = best_f;
                return Result::Success;
            }

            // Проверка границ
            if (!isWithinBounds(x, y)) {
                m_x = best_x;
                m_y = best_y;
                return Result::OutOfBounds;
            }
            std::cout << "Итерация " << m_iterations
                << ": x=" << x << ", y=" << y
                << ", f=" << f_current << std::endl
                << ", ЛУЧШАЯ f=" << best_f << std::endl;
        }
        
        m_x = best_x;
        m_y = best_y;
        f_current = best_f;
        return checkTerminationCondition();
    }

    // Обновление координаты с учётом границ
    double updateCoordinate(double coord, double step,
                            double lower_bound, double upper_bound) {
        double new_coord = coord + step;
        return std::max(lower_bound, std::min(upper_bound, new_coord));
    }

    // Проверка условий завершения
    Result checkTerminationCondition() {
        if (m_iterations >= m_inputData->max_iterations) {
            return Result::MaxIterations;
        }
        if (m_function_calls >= m_inputData->max_function_calls) {
            return Result::MaxFunctionsCalls;
        }
        return Result::Success;
    }
};

} // namespace CD

#endif // COORDINATEDESCENT_COORDINATEDESCENT_HPP_ 
