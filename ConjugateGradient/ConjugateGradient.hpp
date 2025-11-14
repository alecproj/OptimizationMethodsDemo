//
// Created on 25 Oct, 2025
//  by alecproj
//

#ifndef CONJUGATEGRADIENT_CONJUGATEGRADIENT_HPP_
#define CONJUGATEGRADIENT_CONJUGATEGRADIENT_HPP_

#include "ConjugateGradient/Common.hpp"  // Изменено: используем свой Common.hpp
#include <muParser.h>
#include <vector>
#include <cmath>
#include <algorithm>
#include <iostream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace CG {

    template <typename Reporter>
    class ConjugateGradient {

        // Пример объявления и инициализации констант
        static constexpr double MIN_STEP{ 1e-10 };     // Минимальный шаг (до 10^-10)
        static constexpr double STEP_REDUCTION{ 0.5 }; // Коэффициент снижения шага
        static constexpr double MAX_STEP{ 1.0 };       // Максимальный шаг (до 1.0)

    public:

        ConjugateGradient(Reporter* reporter) :
            m_inputData{ nullptr },
            m_reporter{ reporter },
            m_parser{}, // Инициализация парсера muParser 
            m_x{ 0.0 },
            m_y{ 0.0 },
            m_function_calls{ 0 },
            m_iterations{ 0 }
        {
            resetAlgorithmState();
        }

        double getX() const { return m_x; }                             // Получить X
        double getY() const { return m_y; }                             // Получить Y
        int getIterations() const { return m_iterations; }              // Получить кол-во итераций
        int getFunctionCalls() const { return m_function_calls; }       // Получить кол-во вызовов функции
        double getOptimumValue() { return evaluateFunction(m_x, m_y); } // Вычисление значение функции в финальной точке

        Result setInputData(const InputData* data)
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
            if (data->algorithm_type != AlgorithmType::CONJUGATE_GRADIENT) {
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

            // Проверка корректности границ X
            if ((data->x_left_bound >= data->x_right_bound)) {
                return Result::InvalidXBound;
            }

            // Проверка корректности границ Y
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

            // Проверка что точность вычислений меньше точности результата
            if (data->computation_precision > data->result_precision) {
                return Result::InvalidLogicPrecision;
            }

            // --- ВАЛИДАЦИЯ ПАРАМЕТРОВ ШАГА ---
            if (data->constant_step_size <= 0.0) {
                return Result::InvalidConstantStepSize;
            }
            if (data->coefficient_step_size <= 0.0) {
                return Result::InvalidCoefficientStepSize;
            }

            // Сохраняем данные
            m_inputData = data;
            return Result::Success;
        }

        Result solve()
        {
            if (!m_inputData || !m_reporter || m_reporter->begin() != 0) {
                return Result::Fail;
            }
            
            Result result = Result::Success;
            resetAlgorithmState();

            try {
                initializeParser();

                if (!isFunctionDifferentiableAtStart()) {
                    return Result::NonDifferentiableFunction;
                }

                result = conjugateGradient();
            }
            catch (const mu::Parser::exception_type& e) {
                result = Result::ParseError;
            }
            catch (const std::exception& e) {
                result = Result::ComputeError;
            }

            if (m_reporter->end() == 0) {
                return Result::Success;
            }
            return Result::Fail;
        }

    private:

        const InputData* m_inputData;
        Reporter* m_reporter;
        mu::Parser m_parser;
        double m_x, m_y;
        int m_function_calls;
        int m_iterations;
        static constexpr double gradient_epsilon{ 1e-16 };
        std::vector<std::pair<double, double>> m_recent_points;
        int m_oscillation_count;
        std::vector<std::string> m_non_diff_functions;

        // === ВСПОМОГАТЕЛЬНЫЕ МЕТОДЫ ===

        void initializeParser() {
            m_parser.SetExpr(m_inputData->function);
            m_parser.DefineVar("x", &m_x);
            m_parser.DefineVar("y", &m_y);
            m_iterations = 0;
            m_function_calls = 0;
        }

        // Метод для сброса состояния алгоритма
        void resetAlgorithmState() {
            m_function_calls = 0;
            m_iterations = 0;
            m_x = 0.0;
            m_y = 0.0;
            m_recent_points.clear();
            m_oscillation_count = 0;

            // Инициализация списка недифференцируемых функций
            m_non_diff_functions = {
                "abs(", "|", "sign(", "floor(", "ceil(", "round(",
                "fmod(", "mod(", "rand(", "max(", "min(", "random"
            };
        }
        // Проверка синтаксиса функции
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
            }
            catch (const mu::Parser::exception_type& e) {
                return Result::ParseError;
            }
            catch (...) {
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
                m_reporter->insertMessage("Функция не дифференцируема в начальной точке ("
                    + std::to_string(m_inputData->initial_x) 
                    + ", " + std::to_string(m_inputData->initial_y) + ")");
                
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

                for (const auto& non_diff_func : m_non_diff_functions) {
                    std::string non_diff_lower = non_diff_func;
                    std::transform(non_diff_lower.begin(), non_diff_lower.end(), non_diff_lower.begin(), ::tolower);

                    if (func_lower.find(non_diff_lower) != std::string::npos) {
                        std::cout << "Обнаружена потенциально недифференцируемая функция: " << non_diff_func << std::endl;
                        m_reporter->insertMessage("Обнаружена потенциально недифференцируемая функция: " + non_diff_func);
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
                            
                            m_reporter->insertMessage("Производная не определена в точке ("
                                + std::to_string(test_x) + ", " + std::to_string(test_y) + ")");
                            return Result::NonDifferentiableFunction;
                        }
                    }
                    catch (const mu::Parser::exception_type& e) {
                        std::cout << "Функция не дифференцируема в точке ("
                            << test_x << ", " << test_y << "): " << e.GetMsg() << std::endl;
                        
                        m_reporter->insertMessage("Функция не дифференцируема в точке ("
                            + std::to_string(test_x) + ", " + std::to_string(test_y) + ")");
                        return Result::NonDifferentiableFunction;
                    }
                    catch (const std::exception& e) {
                        std::cout << "Ошибка дифференцирования в точке ("
                            << test_x << ", " << test_y << "): " << e.what() << std::endl;

                        m_reporter->insertMessage("Ошибка дифференцирования в точке ("
                            + std::to_string(test_x) + ", " + std::to_string(test_y) + ")");

                        return Result::NonDifferentiableFunction;
                    }
                }
                std::cout << "Функция прошла проверку дифференцируемости" << std::endl;
                m_reporter->insertMessage("Функция прошла проверку дифференцируемости");
                return Result::Success;

            }
            catch (const mu::Parser::exception_type& e) {
                std::cout << "Ошибка парсера при проверке дифференцируемости: " << e.GetMsg() << std::endl;
                m_reporter->insertMessage("Ошибка парсера при проверке дифференцируемости: ");
                return Result::ParseError;
            }
            catch (const std::exception& e) {
                std::cout << "Общая ошибка при проверке дифференцируемости: " << e.what() << std::endl;
                m_reporter->insertMessage("Общая ошибка при проверке дифференцируемости: ");
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
            }
            catch (...) {
                throw std::runtime_error("Ошибка вычисления функции в точке");
            }
        }

        // Вычисление частной производной по X
        double partialDerivativeX(double x, double y) {
            double x_old = m_x, y_old = m_y;
            m_y = y; // Фиксируем y
            double derivative = m_parser.Diff(&m_x, x, m_inputData->computation_precision);
            m_x = x_old;
            m_y = y_old;
            return derivative;
        }

        // Вычисление частной производной по Y  
        double partialDerivativeY(double x, double y) {
            double x_old = m_x, y_old = m_y;
            m_x = x; // Фиксируем x
            double derivative = m_parser.Diff(&m_y, y, m_inputData->computation_precision);
            m_x = x_old;
            m_y = y_old;
            return derivative;
        }

        // Проверка сходимости
        Result checkConvergence(double x_old, double y_old,
            double x_new, double y_new,
            double f_old, double f_new,
            double& best_x, double& best_y, double& best_f) {

            double dx = std::abs(x_new - x_old);
            double dy = std::abs(y_new - y_old);
            double df = std::abs(f_new - f_old);
            double coordinate_norm = std::sqrt(dx * dx + dy * dy);

            // ИСПОЛЬЗУЕМ ЧЛЕНЫ КЛАССА ВМЕСТО СТАТИЧЕСКИХ ПЕРЕМЕННЫХ
            // Сохраняем последние 5 точек
            m_recent_points.push_back({ x_new, y_new });
            if (m_recent_points.size() > 5) {
                m_recent_points.erase(m_recent_points.begin());
            }

            // Проверяем все возможные циклы в последних точках
            if (m_recent_points.size() >= 4) {
                bool found_cycle = false;
                for (size_t i = 0; i < m_recent_points.size() - 2; ++i) {
                    for (size_t j = i + 1; j < m_recent_points.size() - 1; ++j) {
                        double dist = std::sqrt(
                            std::pow(m_recent_points[i].first - m_recent_points[j].first, 2) +
                            std::pow(m_recent_points[i].second - m_recent_points[j].second, 2)
                        );
                        if (dist < m_inputData->computation_precision) {
                            m_oscillation_count++;
                            found_cycle = true;
                            break;
                        }
                    }
                    if (found_cycle) break;
                }

                if (m_oscillation_count > 3) {
                    std::cout << "*** STOP: Oscillation detected after "
                        << m_oscillation_count << " cycles ***" << std::endl;
                    m_reporter->insertMessage("СТОП: Обнаружена осцилляция после " + std::to_string(m_oscillation_count) + " циклов");

                    // ПРИНУДИТЕЛЬНО УСТАНАВЛИВАЕМ ЛУЧШУЮ ТОЧКУ
                    if (m_inputData->extremum_type == ExtremumType::MAXIMUM) {
                        double max_f = best_f;
                        for (const auto& point : m_recent_points) {
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
                        double min_f = best_f;
                        for (const auto& point : m_recent_points) {
                            double f_val = evaluateFunction(point.first, point.second);
                            if (f_val < min_f) {
                                min_f = f_val;
                                best_x = point.first;
                                best_y = point.second;
                                best_f = f_val;
                            }
                        }
                    }
                    return Result::OscillationDetected;
                }

                if (!found_cycle) {
                    m_oscillation_count = 0; // сбрасываем счетчик если цикл прервался
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
                m_reporter->insertMessage("СХОДИМОСТЬ: Координаты и функция стабилизировалась");
                return Result::Success;
            }

            return Result::Continue;
        }

        // Проверка границ (на каждой итерации)
        bool isWithinBounds(double x, double y) {
            return (x >= m_inputData->x_left_bound && x <= m_inputData->x_right_bound &&
                y >= m_inputData->y_left_bound && y <= m_inputData->y_right_bound);
        }

        // ============================================================================
        // РЕАЛИЗАЦИИ ТИПОВ ШАГА
        // ============================================================================

        double getConjugateGradientStep(double x, double y, double dir_x, double dir_y) {
            switch (m_inputData->step_type) {
            case StepType::CONSTANT:
                return m_inputData->constant_step_size;
            case StepType::COEFFICIENT: {
                double dir_norm = std::sqrt(dir_x * dir_x + dir_y * dir_y);
                return m_inputData->coefficient_step_size * dir_norm;
            }
            case StepType::ADAPTIVE:
                return findOptimalStepAlongDirectionCG(x, y, dir_x, dir_y);
            default:
                return m_inputData->constant_step_size;
            }
        }

        double findOptimalStepAlongDirectionCG(double x, double y, double dir_x, double dir_y) {
            const double golden_ratio = 0.618033988749895;
            const double tolerance = 1e-8;
            const int max_iterations = 30;

            double a = 0.0;
            double b = findInitialStepBoundForDirectionCG(x, y, dir_x, dir_y);
            if (b <= a) return m_inputData->constant_step_size;

            double h1 = b - (b - a) * golden_ratio;
            double h2 = a + (b - a) * golden_ratio;

            double f1 = evaluateFunctionAlongDirectionCG(x, y, dir_x, dir_y, h1);
            double f2 = evaluateFunctionAlongDirectionCG(x, y, dir_x, dir_y, h2);

            for (int i = 0; i < max_iterations && (b - a) > tolerance; ++i) {
                if (m_inputData->extremum_type == ExtremumType::MINIMUM) {
                    if (f1 < f2) {
                        b = h2; h2 = h1; f2 = f1;
                        h1 = b - (b - a) * golden_ratio;
                        f1 = evaluateFunctionAlongDirectionCG(x, y, dir_x, dir_y, h1);
                    }
                    else {
                        a = h1; h1 = h2; f1 = f2;
                        h2 = a + (b - a) * golden_ratio;
                        f2 = evaluateFunctionAlongDirectionCG(x, y, dir_x, dir_y, h2);
                    }
                }
                else {
                    if (f1 > f2) {
                        b = h2; h2 = h1; f2 = f1;
                        h1 = b - (b - a) * golden_ratio;
                        f1 = evaluateFunctionAlongDirectionCG(x, y, dir_x, dir_y, h1);
                    }
                    else {
                        a = h1; h1 = h2; f1 = f2;
                        h2 = a + (b - a) * golden_ratio;
                        f2 = evaluateFunctionAlongDirectionCG(x, y, dir_x, dir_y, h2);
                    }
                }
            }
            return (a + b) / 2.0;
        }

        double evaluateFunctionAlongDirectionCG(double x, double y, double dir_x, double dir_y, double step) {
            double x_new = x + step * dir_x;
            double y_new = y + step * dir_y;
            
            x_new = std::max(m_inputData->x_left_bound, std::min(m_inputData->x_right_bound, x_new));
            y_new = std::max(m_inputData->y_left_bound, std::min(m_inputData->y_right_bound, y_new));
            return evaluateFunction(x_new, y_new);
        }

        // Нахождение начальной границы для шага в CG
            double findInitialStepBoundForDirectionCG(double x, double y, double dir_x, double dir_y) {
                
                double max_step = 1.0;

                // Адаптивно подбираем максимальный шаг на основе направления
                double dir_norm = std::sqrt(dir_x * dir_x + dir_y * dir_y);
                if (dir_norm > 1e-10) {
                    max_step = std::min(1.0, 0.5 / dir_norm);
                }

                // Убеждаемся, что шаг не выходит за границы
                int safety_counter = 0;
                while (max_step > 1e-10 && safety_counter < 20) {
                    double test_x = x + max_step * dir_x;
                    double test_y = y + max_step * dir_y;

                    if (isWithinBounds(test_x, test_y)) {
                        // Дополнительная проверка: функция должна улучшаться
                        double current_f = evaluateFunction(x, y);
                        double new_f = evaluateFunction(test_x, test_y);

                        bool improvement = (m_inputData->extremum_type == ExtremumType::MINIMUM)
                            ? (new_f < current_f)
                            : (new_f > current_f);

                        if (improvement || safety_counter > 10) {
                            break;
                        }
                    }
                    max_step *= 0.7;
                    safety_counter++;
                }

                return max_step;
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
                m_reporter->insertMessage("Достигнуто максимальное количество итераций");
                return Result::MaxIterations;
            }
            if (m_function_calls >= m_inputData->max_function_calls) {
                m_reporter->insertMessage("Достигнуто максимальное количество вызовов функции");
                return Result::MaxFunctionsCalls;
            }
            return Result::Success;
        }

        void ReporterResult(double best_x, double best_y, double best_f, int m_function_calls, int m_iterations) {

            m_reporter->insertMessage("Итого:");
            m_reporter->insertMessage("Количество итераций: " + std::to_string(m_iterations));
            m_reporter->insertMessage("Количество вызовов функции: " + std::to_string(m_function_calls));
            m_reporter->insertResult(best_x, best_y, best_f);
        }

        //Считает количество знаков после запятой
        static int countDecimals(const double value) {
            std::string s = std::to_string(value);
            size_t pos = s.find('.');
            if (pos == std::string::npos) return 0;

            while (!s.empty() && s.back() == '0') s.pop_back();
            return s.size() - pos - 1;
        }

        //Округляет число до указанного количества знаков после запятой
        double roundTo(const double value, const int digits) {
            double factor = pow(10.0, digits);
            //return round(value * factor) / factor;
            return ceil(value * factor) / factor;
        }

        // Метод сопряженных градиентов (Fletcher-Reeves)
        Result conjugateGradient() {

            double x = m_inputData->initial_x;
            double y = m_inputData->initial_y;
            double f_current = evaluateFunction(x, y);

            double best_x = x, best_y = y, best_f = f_current;
            m_iterations = 0;

            // Начальный градиент
            double grad_x = partialDerivativeX(x, y);
            double grad_y = partialDerivativeY(x, y);
            double grad_norm_old = grad_x * grad_x + grad_y * grad_y;

            auto iterationTable = m_reporter->beginTable("Метод сопряженных градиентов ",
                { "i", "x", "y", "f(x,y)", "∇f/∂x", "∇f/∂y", "Шаг", "β", "||∇f||" });
            
            
            double direction_sign = (m_inputData->extremum_type == ExtremumType::MINIMUM) ? -1.0 : 1.0;
            double direction_x = direction_sign * grad_x;
            double direction_y = direction_sign * grad_y;
            
            std::cout << "=== ЗАПУСК CONJUGATE GRADIENT ===" << std::endl;
            std::cout << "Начальная точка: (" << x << ", " << y << "), f = " << f_current << std::endl;

            while (m_iterations < m_inputData->max_iterations &&
                m_function_calls < m_inputData->max_function_calls) {

                double x_old = x, y_old = y;
                double f_old = f_current;

                // 1. Поиск оптимального шага вдоль текущего направления
                double optimal_step = findOptimalStepAlongDirectionCG(x, y, direction_x, direction_y);

                // 2. Обновление координат
                x = updateCoordinate(x, optimal_step * direction_x,
                    m_inputData->x_left_bound, m_inputData->x_right_bound);
                y = updateCoordinate(y, optimal_step * direction_y,
                    m_inputData->y_left_bound, m_inputData->y_right_bound);

                f_current = evaluateFunction(x, y);
                m_iterations++;

                // 3. Вычисление нового градиента
                double new_grad_x = partialDerivativeX(x, y);
                double new_grad_y = partialDerivativeY(x, y);

                // 4. Вычисление коэффициента Флетчера-Ривза
                double grad_norm_new = new_grad_x * new_grad_x + new_grad_y * new_grad_y;
                double beta = (m_iterations % 2 == 0) ? 0.0 : (grad_norm_new / grad_norm_old); // Сброс каждые 2 итерации

                // 5. Обновление сопряженного направления
                direction_x = direction_sign * new_grad_x + beta * direction_x;
                direction_y = direction_sign * new_grad_y + beta * direction_y;

                grad_norm_old = grad_norm_new;

                // Обновление лучшей точки
                bool improvement = (m_inputData->extremum_type == ExtremumType::MINIMUM)
                    ? (f_current < best_f)
                    : (f_current > best_f);

                if (improvement) {
                    best_x = x;
                    best_y = y;
                    best_f = f_current;
                }

             m_reporter->insertRow(iterationTable, {
                m_iterations,
                x, y, f_current,
                new_grad_x, new_grad_y,
                optimal_step, beta,
                std::sqrt(grad_norm_new)
             });
                // Отладочный вывод
                /*std::cout << "Итерация " << m_iterations
                    << ": x=" << x << ", y=" << y
                    << ", f=" << f_current
                    << ", grad_norm=" << std::sqrt(grad_norm_new)
                    << ", beta=" << beta
                    << ", ЛУЧШАЯ f=" << best_f << std::endl;*/

                // Проверка границ
                if (!isWithinBounds(x, y)) {
                    m_x = best_x;
                    m_y = best_y;
                    std::cout << "=== CONJUGATE GRADIENT: ВЫХОД ЗА ГРАНИЦЫ ===" << std::endl;
                    return Result::OutOfBounds;
                }
                
                Result conv = checkConvergence(x_old, y_old, x, y, f_old, f_current, best_x, best_y, best_f);
                if (conv != Result::Continue) {
                    m_x = best_x; m_y = best_y;
                    m_reporter->endTable(iterationTable);

                    switch (conv) {
                    case Result::Success:
                        m_reporter->insertMessage("✅Алгоритм завершен: Сходимость достигнута");
                        break;
                    case Result::OscillationDetected:
                        m_reporter->insertMessage("✅Алгоритм завершен: обнаружены осцилляции — возвращена лучшая точка");
                        break;
                    default:
                        m_reporter->insertMessage("🔴Останов по коду: " + std::to_string(static_cast<int>(conv)));
                        break;
                    }

                    ReporterResult(best_x, best_y, best_f, m_function_calls, m_iterations);
                    m_reporter->insertResult(best_x, best_y, best_f);
                    return conv;
                }

                

                // Проверка на слишком маленький градиент
                if (std::sqrt(grad_norm_new) < m_inputData->computation_precision) {
                    std::cout << "=== CONJUGATE GRADIENT: ГРАДИЕНТ СЛИШКОМ МАЛ ===" << std::endl;
                    m_x = best_x;
                    m_y = best_y;
                    m_reporter->endTable(iterationTable);
                    m_reporter->insertMessage("Алгоритм завершен: Градиаент слишком мал");
                    ReporterResult(best_x, best_y, best_f, m_function_calls, m_iterations);
                    m_reporter->insertResult(best_x, best_y, best_f);
                    return Result::Success;
                }
            }

            m_x = best_x; m_y = best_y;
            m_reporter->endTable(iterationTable);
            Result term = checkTerminationCondition();
            ReporterResult(best_x, best_y, best_f, m_function_calls, m_iterations);
            m_reporter->insertResult(best_x, best_y, best_f);
            return term;
        }
    };

} // namespace CG

#endif // CONJUGATEGRADIENT_CONJUGATEGRADIENT_HPP_
