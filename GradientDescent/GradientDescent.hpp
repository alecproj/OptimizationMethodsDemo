#ifndef GRADIENTDESCENT_GRADIENTDESCENT_HPP_
#define GRADIENTDESCENT_GRADIENTDESCENT_HPP_

#include <GradientDescent/Common.hpp>
#include <glog/logging.h>
#include <muParser.h>
#include <vector>
#include <cmath>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef DEBUG
#define DEBUG 1
#endif

namespace GD {

template <typename Reporter>
class GradientDescent {

    // Пример объявления и инициализации констант
    static constexpr double MIN_STEP{ 1e-10 };       // Минимальный шаг (до 10^-10)
    static constexpr double STEP_REDUCTION{ 0.5 };   // Коэффициент снижения шага
    static constexpr double MAX_STEP{ 1.0 };         // Максимальный шаг (до 1.0)

public:

    GradientDescent(Reporter* reporter) :
        m_inputData{ nullptr },
        m_reporter{ reporter },
        m_parser{}, // Инициализация парсера muParser
        m_x{ 0.0 },
        m_y{ 0.0 },
        m_function_calls{ 0 },
        m_iterations{ 0 },
        m_computationPrecision{ 0.0 },
        m_resultPrecision{0.0}
    {
    }

    // Добавьте метод для сброса состояния
    void reset() {
        m_x = 0.0;
        m_y = 0.0;
        m_function_calls = 0;
        m_iterations = 0;
        m_digitResultPrecision = 0;
        m_digitComputationPrecision = 0;
        m_oscillation_count = 0;
        m_recent_points.clear();
        // Очищаем парсер
        m_parser = mu::Parser{};
        m_computationPrecision = 0.0;
        m_resultPrecision = 0.0;
    }


    double getX() const { return m_x; }                        // Получить X
    double getY() const { return m_y; }                        // Получить Y
    int getIterations() const { return m_iterations; }               // Получить кол-во итераций
    int getFunctionCalls() const { return m_function_calls; }           // Получить кол-во вызовов функции
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
        if (data->algorithm_type != AlgorithmType::GRADIENT_DESCENT &&
            data->algorithm_type != AlgorithmType::STEEPEST_DESCENT &&
            data->algorithm_type != AlgorithmType::RAVINE_METHOD) {
            return Result::InvalidAlgorithmType;
        }

        // Проверка типа экстремума
        if (data->extremum_type != ExtremumType::MINIMUM &&
            data->extremum_type != ExtremumType::MAXIMUM) {
            return Result::InvalidExtremumType;
        }

        // Проверка типа шага
        if (data->algorithm_type == AlgorithmType::GRADIENT_DESCENT) {
            if (data->step_type != StepType::CONSTANT &&
                data->step_type != StepType::COEFFICIENT &&
                data->step_type != StepType::ADAPTIVE) {
                return Result::InvalidStepType;
            }
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
        if (data->result_precision < 1 || data->result_precision > 15) {
            return Result::InvalidResultPrecision;
        }

        // Проверка точности вычислений
        if (data->computation_precision < 1 || data->computation_precision > 15) {
            return Result::InvalidComputationPrecision;
        }

        // Проверка, что точность вычислений меньше точности результата
        if (data->computation_precision < data->result_precision) {
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
        if (data->constant_step_size <= 0.0) {
            return Result::InvalidConstantStepSizeX;
        }
        if (data->coefficient_step_size <= 0.0) {
            return Result::InvalidCoefficientStepSizeX;
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

        reset();

        m_computationPrecision = std::pow(10, -m_inputData->computation_precision);
        m_resultPrecision = std::pow(10, -m_inputData->result_precision);
        m_digitResultPrecision = m_inputData->result_precision;
        m_digitComputationPrecision = m_inputData->computation_precision;


        m_reporter->insertMessage("Начало заполнения отчета.");
        Result result = Result::Success;

        try {
            // Инициализация Парсера
            initializeParser();

            if (!isFunctionDifferentiableAtStart()) {
                return Result::NonDifferentiableFunction;
            }

            // Выбор алгоритма
            switch (m_inputData->algorithm_type) {
            case AlgorithmType::GRADIENT_DESCENT:
                result = gradientDescent();
                break;
            case AlgorithmType::STEEPEST_DESCENT:
                result = steepestDescent();
                break;
            case AlgorithmType::RAVINE_METHOD:
                result = ravineMethod();
                break;
            default:
                result = Result::InvalidAlgorithmType;
                break;
            }
        }
        catch (const mu::Parser::exception_type& e) {
            result = Result::ParseError;
        }
        catch (const std::exception& e) {
            result = Result::ComputeError;
        }
        /*
        if (m_reporter->end() == 0) {
            return GDResult::Success;
        }
        return GDResult::Fail;
        */
        if (m_reporter->end() == 0) {
            return result;
        }else {
            if (result == Result::Success) return Result::Fail;
            else return result;
        }
    }

private:

    const InputData* m_inputData; // Настройки алгоритма
    Reporter* m_reporter; // Указатель на систему отчётности
    mu::Parser m_parser; // Система вычисления
    double m_x, m_y; // Текущие переменные для парсера
    int m_function_calls; // Счётчик вызовов функции
    int m_iterations; // Счётчик итераций
    int m_digitResultPrecision; //Количество знаков после запятой для результата
    int m_digitComputationPrecision; //Количество знаков после запятой для вычисленийo
    double m_computationPrecision;
    double m_resultPrecision;

    std::vector<std::pair<double, double>> m_recent_points;
    int m_oscillation_count = 0;

    // Инициализация парсера
    void initializeParser() {
        m_parser.SetExpr(m_inputData->function); // Загрузка заданной функции
        m_parser.DefineVar("x", &m_x); // Связывание переменной X с полем m_x
        m_parser.DefineVar("y", &m_y); // Связывание переменной Y с полем m_y
        m_function_calls = 0;
        m_iterations = 0;
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
            //m_reporter->insertMessage("Функция не дифференцируема в начальной точке (" + std::to_string(m_inputData->initial_x) + ", " + std::to_string(m_inputData->initial_y) + ")");
            LOG(ERROR) << "Функция не дифференцируема в начальной точке (" << m_inputData->initial_x << ", " << m_inputData->initial_y << ")";
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
                    LOG(ERROR) << "Обнаружена потенциально недифференцируемая функция: " << non_diff_func;
                    LOG(ERROR) << "Функция содержит: " << function;
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
                        LOG(ERROR) << "Производная не определена в точке ("
                            << test_x << ", " << test_y << ")";
                        return Result::NonDifferentiableFunction;
                    }
                }
                catch (const mu::Parser::exception_type& e) {
                    LOG(ERROR) << "Функция не дифференцируема в точке ("
                        << test_x << ", " << test_y << "): " << e.GetMsg();
                    return Result::NonDifferentiableFunction;
                }
                catch (const std::exception& e) {
                    LOG(ERROR) << "Ошибка дифференцирования в точке ("
                        << test_x << ", " << test_y << "): " << e.what();
                    return Result::NonDifferentiableFunction;
                }
            }
            //m_reporter->insertMessage("Функция прошла проверку дифференцируемости");
            VLOG(DEBUG) << "Функция прошла проверку дифференцируемости";
            return Result::Success;

        }
        catch (const mu::Parser::exception_type& e) {
            LOG(ERROR) << "Ошибка парсера при проверке дифференцируемости: " << e.GetMsg();
            return Result::ParseError;
        }
        catch (const std::exception& e) {
            LOG(ERROR) << "Общая ошибка при проверке дифференцируемости: " << e.what();
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
        double derivative = m_parser.Diff(&m_x, x, m_computationPrecision);
        m_x = x_old;
        m_y = y_old;
        return derivative;
    }

    // Вычисление частной производной по Y
    double partialDerivativeY(double x, double y) {
        double x_old = m_x, y_old = m_y;
        m_x = x; // Фиксируем x
        double derivative = m_parser.Diff(&m_y, y, m_computationPrecision);
        m_x = x_old;
        m_y = y_old;
        return derivative;
    }

// Проверка сходимости
// Проверка сходимости
Result checkConvergence(double x_old, double y_old,
    double x_new, double y_new,
    double f_old, double f_new,
    double& best_x, double& best_y, double& best_f) {

    double dx = std::abs(x_new - x_old);
    double dy = std::abs(y_new - y_old);
    double df = std::abs(f_new - f_old);

    double coordinate_norm = std::sqrt(dx * dx + dy * dy);

    // ИСПОЛЬЗУЕМ ПОЛЯ КЛАССА вместо static переменных
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
                if (dist < m_computationPrecision) {
                    m_oscillation_count++;
                    found_cycle = true;
                    break;
                }
            }
            if (found_cycle) break;
        }

        if (m_oscillation_count > 3) { // уменьшил порог для более раннего обнаружения
            LOG(INFO) << "*** STOP: Oscillation detected after "
                << m_oscillation_count << " cycles ***";
            m_reporter->insertMessage("СТОП: Обнаружена осцилляция после " + std::to_string(m_oscillation_count) + " циклов");

            // ПРИНУДИТЕЛЬНО УСТАНАВЛИВАЕМ ЛУЧШУЮ ТОЧКУ
            if (m_inputData->extremum_type == ExtremumType::MAXIMUM) {
                // Для максимума ищем точку с наибольшим значением функции
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
                // Для минимума ищем точку с наименьшим значением функции
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
    if (coordinate_norm < m_resultPrecision &&
        df < m_resultPrecision) {

        // ПЕРЕД ВОЗВРАТОМ УБЕДИТЕСЬ, ЧТО ИСПОЛЬЗУЕМ ЛУЧШУЮ ТОЧКУ
        double current_f = evaluateFunction(x_new, y_new);
        if ((m_inputData->extremum_type == ExtremumType::MAXIMUM && current_f > best_f) ||
            (m_inputData->extremum_type == ExtremumType::MINIMUM && current_f < best_f)) {
            best_x = x_new;
            best_y = y_new;
            best_f = current_f;
        }

        LOG(INFO) << "*** CONVERGENCE: Coordinates and function stabilized ***";
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

    // ============================================================================
    // ГРАДИЕНТНЫЕ АЛГОРИТМЫ ОПТИМИЗАЦИИ
    // ============================================================================

    // Базовый градиентный спуск
    Result gradientDescent() {
        double x = roundComputation(m_inputData->initial_x);
        double y = roundComputation(m_inputData->initial_y);
        double f_current = roundComputation(evaluateFunction(x, y));

        double best_x = roundComputation(x), best_y = roundComputation(y), best_f = roundComputation(f_current);
        m_iterations = 0;

        LOG(INFO) << "=== ЗАПУСК GRADIENT DESCENT ===";
        VLOG(DEBUG) << "Начальная точка: (" << x << ", " << y << "), f = " << f_current;

        auto iterationTable = m_reporter->beginTable("Шаги запуска", {"Номер итерации i", "x_i", "y_i", "f_i", "Градиент", "Шаг"});


        while (m_iterations < m_inputData->max_iterations &&
            m_function_calls < m_inputData->max_function_calls) {

            double x_old = roundComputation(x), y_old = roundComputation(y);
            double f_old = roundComputation(f_current);

            // 1. ВЫЧИСЛЯЕМ ГРАДИЕНТ
            double grad_x = roundComputation(partialDerivativeX(x, y));
            double grad_y = roundComputation(partialDerivativeY(x, y));

            double grad_norm = roundComputation(std::sqrt(grad_x * grad_x + grad_y * grad_y));

            // 2. ВЫБИРАЕМ ШАГ ДЛЯ ВСЕХ КООРДИНАТ
            double step = roundComputation(getGradientStepSize(x, y, grad_x, grad_y, grad_norm));

            // 3. ДВИЖЕНИЕ ПО ВСЕМ КООРДИНАТАМ ОДНОВРЕМЕННО
            double direction = (m_inputData->extremum_type == ExtremumType::MINIMUM) ? -1.0 : 1.0;
            double delta_x = direction * step * grad_x;
            double delta_y = direction * step * grad_y;

            VLOG(DEBUG) << "  grad_x = " << grad_x << ", grad_y = " << grad_y;
            VLOG(DEBUG) << "  step = " << step;
            VLOG(DEBUG) << "  delta_x = " << delta_x << ", delta_y = " << delta_y;
            VLOG(DEBUG) << "  before: x = " << x << ", y = " << y;

            x = updateCoordinate(x, direction * step * grad_x, m_inputData->x_left_bound, m_inputData->x_right_bound);
            y = updateCoordinate(y, direction * step * grad_y, m_inputData->y_left_bound, m_inputData->y_right_bound);

            f_current = roundComputation(evaluateFunction(x, y));
            m_iterations++;

            // Обновление лучшей точки
            bool improvement = (m_inputData->extremum_type == ExtremumType::MINIMUM)
                ? (f_current < best_f)
                : (f_current > best_f);

            if (improvement) {
                best_x = roundComputation(x);
                best_y = roundComputation(y);
                best_f = roundComputation(f_current);
            }


            m_reporter->insertRow(iterationTable,{m_iterations,roundComputation(x), roundComputation(y), roundComputation(f_current), roundComputation(grad_norm), roundComputation(step)});

            VLOG(DEBUG) << "Итерация " << m_iterations
                << ": x=" << x << ", y=" << y
                << ", f=" << f_current
                << ", grad_norm=" << grad_norm
                << ", step=" << step
                << ", ЛУЧШАЯ f=" << best_f;
            VLOG(DEBUG) << "NORM: " << grad_norm << " CP: " << m_computationPrecision;

            // Проверка сходимости
            Result conv = checkConvergence(x_old, y_old, x, y, f_old, f_current, best_x, best_y, best_f);
            if (conv != Result::Continue) {
                m_x = roundComputation(best_x);
                m_y = roundComputation(best_y);
                m_reporter->endTable(iterationTable);

                switch (conv) {
                    case Result::Success:
                        m_reporter->insertMessage("Сходимость достигнута. Базовый градиентный метод завершен.");
                        break;
                    case Result::OscillationDetected:
                        m_reporter->insertMessage("Алгоритм завершен: обнаружены осцилляции — возвращена лучшая точка");
                        break;
                    default:
                        m_reporter->insertMessage("Остановка по коду: " + std::to_string(static_cast<int>(conv)));
                        break;
                }

                ReporterResult(roundResult(best_x), roundResult(best_y) , roundResult(best_f), m_function_calls, m_iterations);
                m_reporter->insertResult(roundResult(best_x), roundResult(best_y), roundResult(best_f));
                return conv;
            }

            // Проверка границ
            if (!isWithinBounds(x, y)) {
                m_x = roundResult(best_x);
                m_y = roundResult(best_y);

                m_reporter->endTable(iterationTable);
                m_reporter->insertMessage("Базовый градиентный метод не завершен выход за границы.");
                LOG(ERROR) << "=== GRADIENT DESCENT: ВЫХОД ЗА ГРАНИЦЫ ===";

                return Result::OutOfBounds;
            }
            // Проверка на слишком маленький градиент
            if (grad_norm < m_computationPrecision) {


                m_reporter->endTable(iterationTable);
                m_reporter->insertMessage("Базовый градиентный метод - градиент слишком мал.");
                ReporterResult(roundResult(best_x), roundResult(best_y) , roundResult(best_f), m_function_calls, m_iterations);
                m_reporter->insertResult(roundResult(best_x), roundResult(best_y), roundResult(best_f));
                LOG(INFO) << "=== GRADIENT DESCENT: ГРАДИЕНТ СЛИШКОМ МАЛ ===";
                m_x = roundResult(best_x);
                m_y = roundResult(best_y);
                return Result::Success;
            }
        }

        m_x = roundResult(best_x);
        m_y = roundResult(best_y);
        m_reporter->insertMessage("Базовый градиентный метод - достигнуты ограничения.");
        LOG(INFO) << "=== GRADIENT DESCENT: ДОСТИГНУТЫ ОГРАНИЧЕНИЯ ===";
        return checkTerminationCondition();
    }

    // Наискорейший спуск с подбором шага
    Result steepestDescent() {
        double x = roundComputation(m_inputData->initial_x);
        double y = roundComputation(m_inputData->initial_y);
        double f_current = roundComputation(evaluateFunction(x, y));

        double best_x = roundComputation(x), best_y = roundComputation(y), best_f = roundComputation(f_current);
        m_iterations = 0;

        LOG(INFO) << "=== ЗАПУСК STEEPEST DESCENT ===";
        VLOG(DEBUG) << "Начальная точка: (" << x << ", " << y << "), f = " << f_current;

        auto iterationTable = m_reporter->beginTable("Шаги запуска", {"Номер итерации i", "x_i", "y_i", "f_i", "Градиент", "Оптимальный шаг"});

        while (m_iterations < m_inputData->max_iterations &&
            m_function_calls < m_inputData->max_function_calls) {

            double x_old = roundComputation(x), y_old = roundComputation(y);
            double f_old = roundComputation(f_current);

            // 1. ВЫЧИСЛЯЕМ ГРАДИЕНТ
            double grad_x = roundComputation(partialDerivativeX(x, y));
            double grad_y = roundComputation(partialDerivativeY(x, y));

            double grad_norm = roundComputation(std::sqrt(grad_x * grad_x + grad_y * grad_y));

            // 2. НАХОДИМ ОПТИМАЛЬНЫЙ ШАГ ВДОЛЬ НАПРАВЛЕНИЯ ГРАДИЕНТА
            double optimal_step = roundComputation(findOptimalStepAlongGradient(x, y, grad_x, grad_y));

            // 3. ДВИЖЕНИЕ ПО ОПТИМАЛЬНОМУ ШАГУ
            double direction = (m_inputData->extremum_type == ExtremumType::MINIMUM) ? -1.0 : 1.0;

            x = roundComputation(updateCoordinate(x, direction * optimal_step * grad_x, m_inputData->x_left_bound, m_inputData->x_right_bound));
            y = roundComputation(updateCoordinate(y, direction * optimal_step * grad_y, m_inputData->y_left_bound, m_inputData->y_right_bound));

            f_current = roundComputation(evaluateFunction(x, y));
            m_iterations++;

            // Обновление лучшей точки
            bool improvement = (m_inputData->extremum_type == ExtremumType::MINIMUM)
                ? (f_current < best_f)
                : (f_current > best_f);

            if (improvement) {
                best_x = roundComputation(x);
                best_y = roundComputation(y);
                best_f = roundComputation(f_current);
            }
            m_reporter->insertRow(iterationTable,{m_iterations, x, y, f_current, grad_norm, optimal_step});

            VLOG(DEBUG) << "Итерация " << m_iterations
                << ": x=" << x << ", y=" << y
                << ", f=" << f_current
                << ", grad_norm=" << grad_norm
                << ", optimal_step=" << optimal_step;

            // Проверка сходимости
            Result conv = checkConvergence(x_old, y_old, x, y, f_old, f_current, best_x, best_y, best_f);
            if (conv != Result::Continue) {
                m_x = roundComputation(best_x);
                m_y = roundComputation(best_y);
                m_reporter->endTable(iterationTable);

                switch (conv) {
                    case Result::Success:
                        m_reporter->insertMessage("Сходимость достигнута. Метод наискорейшего спуска для градиентного метода завершен.");
                        ReporterResult(roundResult(best_x), roundResult(best_y) , roundResult(best_f), m_function_calls, m_iterations);
                        m_reporter->insertResult(roundResult(best_x), roundResult(best_y), roundResult(best_f));
                        break;
                    case Result::OscillationDetected:
                        m_reporter->insertMessage("Алгоритм завершен: обнаружены осцилляции — возвращена лучшая точка");
                        ReporterResult(roundResult(best_x), roundResult(best_y) , roundResult(best_f), m_function_calls, m_iterations);
                        m_reporter->insertResult(roundResult(best_x), roundResult(best_y), roundResult(best_f));
                        break;
                    default:
                        m_reporter->insertMessage("Остановка по коду: " + std::to_string(static_cast<int>(conv)));
                        ReporterResult(roundResult(best_x), roundResult(best_y) , roundResult(best_f), m_function_calls, m_iterations);
                        m_reporter->insertResult(roundResult(best_x), roundResult(best_y), roundResult(best_f));
                        break;
                }
                return conv;
            }

            // Проверка границ
            if (!isWithinBounds(x, y)) {
                m_x = roundComputation(best_x);
                m_y = roundComputation(best_y);
                m_reporter->endTable(iterationTable);
                m_reporter->insertMessage("Метод наискорейшего спуска для градиентного метода завершен - выход за границы.");
                LOG(ERROR) << "=== STEEPEST DESCENT: ВЫХОД ЗА ГРАНИЦЫ ===";
                return Result::OutOfBounds;
            }

            // Проверка на слишком маленький градиент
            if (grad_norm < m_computationPrecision) {

                m_x = roundComputation(best_x);
                m_y = roundComputation(best_y);
                m_reporter->endTable(iterationTable);

                m_reporter->insertMessage("Метод наискорейшего спуска для градиентного метода завершен - градиент слишком мал.");
                ReporterResult(roundResult(best_x), roundResult(best_y) , roundResult(best_f), m_function_calls, m_iterations);
                m_reporter->insertResult(roundResult(best_x), roundResult(best_y), roundResult(best_f));

                LOG(INFO) << "=== STEEPEST DESCENT: ГРАДИЕНТ СЛИШКОМ МАЛ ===";
                return Result::Success;
            }
        }

        m_x = roundComputation(best_x);
        m_y = roundComputation(best_y);
        m_reporter->endTable(iterationTable);
        m_reporter->insertMessage("Метод наискорейшего спуска для градиентного метода завершен - достигнуты ограничения.");
        LOG(INFO) << "=== STEEPEST DESCENT: ДОСТИГНУТЫ ОГРАНИЧЕНИЯ ===";
        return checkTerminationCondition();
    }

    // Основная реализация овражного метода
    Result ravineMethod() {
        double x = roundComputation(m_inputData->initial_x);
        double y = roundComputation(m_inputData->initial_y);
        double f_current = roundComputation(evaluateFunction(x, y));

        double best_x = roundComputation(x), best_y = roundComputation(y), best_f = roundComputation(f_current);
        m_iterations = 0;

        // История для детекции оврагов
        std::vector<std::pair<double, double>> trajectory;
        std::vector<std::pair<double, double>> gradient_history;
        trajectory.push_back({ x, y });

        // Параметры овражного метода
        double ravine_factor = 0.0; // 0 - нет оврага, 1 - сильный овраг
        const int history_size = 5;

        auto iterationTable = m_reporter->beginTable("Шаги запуска", {"Номер итерации i", "x_i", "y_i", "f_i", "Градиент норм", "Ravine factor"});

        while (m_iterations < m_inputData->max_iterations &&
            m_function_calls < m_inputData->max_function_calls) {

            double x_old = roundComputation(x), y_old = roundComputation(y);
            double f_old = roundComputation(f_current);

            // 1. Вычисляем градиент
            double grad_x = roundComputation(partialDerivativeX(x, y));
            double grad_y = roundComputation(partialDerivativeY(x, y));
            double grad_norm = roundComputation(std::sqrt(grad_x * grad_x + grad_y * grad_y));

            if (grad_norm < m_computationPrecision) {
                m_x = roundResult(best_x);
                m_y = roundResult(best_y);

                LOG(INFO) << "=== RAVINE METHOD ЗАВЕРШЕН (экстремум найден) ===";
                m_reporter->insertMessage("Овражный метод завершён — достигнут экстремум.");
                ReporterResult(roundResult(best_x), roundResult(best_y) , roundResult(best_f), m_function_calls, m_iterations);
                m_reporter->insertResult(roundResult(best_x), roundResult(best_y), roundResult(best_f));
                return Result::Success;

            }

            gradient_history.push_back({ grad_x, grad_y });
            if (gradient_history.size() > history_size) {
                gradient_history.erase(gradient_history.begin());
            }

            // 2. Улучшенная детекция оврагов
            ravine_factor = roundComputation(detectRavineFactor(gradient_history, trajectory));

            // 3. Комбинированное направление
            double dir_x, dir_y;
            if (ravine_factor > 0.3) {
                // Сильный овраг - используем направление вдоль оврага
                computeRavineDirection(trajectory, dir_x, dir_y);
            }
            else if (ravine_factor > 0.1) {
                // Слабый овраг - комбинируем направления
                computeCombinedDirection(grad_x, grad_y, trajectory,
                    ravine_factor, dir_x, dir_y);
            }
            else {
                // Нет оврага - обычный градиент
                dir_x = roundComputation(grad_x / grad_norm);
                dir_y = roundComputation(grad_y / grad_norm);
            }

            // 4. Поиск оптимального шага
            double direction_sign = (m_inputData->extremum_type == ExtremumType::MINIMUM) ? -1.0 : 1.0;
            double optimal_step = roundComputation(findOptimalStepAlongDirection(x, y, dir_x, dir_y, direction_sign));

            // 5. Обновление координат
            x = roundComputation(updateCoordinate(x, direction_sign * optimal_step * dir_x,
                m_inputData->x_left_bound, m_inputData->x_right_bound));
            y = roundComputation(updateCoordinate(y, direction_sign * optimal_step * dir_y,
                m_inputData->y_left_bound, m_inputData->y_right_bound));

            f_current = roundComputation(evaluateFunction(x, y));
            m_iterations++;
            trajectory.push_back({ x, y });

            if (trajectory.size() > history_size) {
                trajectory.erase(trajectory.begin());
            }

            // Обновление лучшей точки
            bool improvement = (m_inputData->extremum_type == ExtremumType::MINIMUM)
                ? (f_current < best_f)
                : (f_current > best_f);

            if (improvement) {
                best_x = roundComputation(x);
                best_y = roundComputation(y);
                best_f = roundComputation(f_current);
            }

            m_reporter->insertRow(iterationTable,{m_iterations, x, y, f_current, grad_norm, ravine_factor});
             // Отладочный вывод
            VLOG(DEBUG) << "Итерация " << m_iterations
                << ": x=" << x << ", y=" << y
                << ", f=" << f_current
                << ", grad_norm=" << grad_norm
                << ", ravine_factor=" << ravine_factor
                << ", ЛУЧШАЯ f=" << best_f;
            // Проверка сходимости
            Result conv = checkConvergence(x_old, y_old, x, y, f_old, f_current, best_x, best_y, best_f);
            if (conv != Result::Continue) {
                m_x = roundResult(best_x);
                m_y = roundResult(best_y);
                m_reporter->endTable(iterationTable);

                switch (conv) {
                    case Result::Success:
                        m_reporter->insertMessage("Сходимость достигнута. Овражное расширение градиентного спуска завершено.");
                        ReporterResult(roundResult(best_x), roundResult(best_y) , roundResult(best_f), m_function_calls, m_iterations);
                        m_reporter->insertResult(roundResult(best_x), roundResult(best_y), roundResult(best_f));
                        break;
                    case Result::OscillationDetected:
                        m_reporter->insertMessage("Алгоритм завершен: обнаружены осцилляции — возвращена лучшая точка");
                        ReporterResult(roundResult(best_x), roundResult(best_y) , roundResult(best_f), m_function_calls, m_iterations);
                        m_reporter->insertResult(roundResult(best_x), roundResult(best_y), roundResult(best_f));
                        break;
                    default:
                        m_reporter->insertMessage("Остановка по коду: " + std::to_string(static_cast<int>(conv)));
                        break;
                }


                return conv;
            }

            // Проверка границ
            if (!isWithinBounds(x, y)) {
                m_x = roundResult(best_x);
                m_y = roundResult(best_y);
                LOG(ERROR) << "=== RAVINE METHOD: ВЫХОД ЗА ГРАНИЦЫ ===";

                m_reporter->insertMessage("Овражное расширение градиентного спуска завершено - выход за границы.");

                return Result::OutOfBounds;
            }
        }

        m_x = roundResult(best_x);
        m_y = roundResult(best_y);
        m_reporter->insertMessage("Овражное расширение градиентного спуска завершено - достигнуты ограничения.");
        LOG(INFO) << "=== RAVINE METHOD: ДОСТИГНУТЫ ОГРАНИЧЕНИЯ ===";
        return checkTerminationCondition();
    }

    // ============================================================================
    // ВСПОМОГАТЕЛЬНЫЕ МЕТОДЫ ДЛЯ ГРАДИЕНТНЫХ АЛГОРИТМОВ
    // ============================================================================

    // Выбор шага для градиентного метода
    double getGradientStepSize(double x, double y, double grad_x, double grad_y, double grad_norm) {
        // Используем шаг для X как основной
        StepType step_type = m_inputData->step_type;

        switch (step_type) {
        case StepType::CONSTANT:
            return m_inputData->constant_step_size;

        case StepType::COEFFICIENT:
            return m_inputData->coefficient_step_size * grad_norm;

        case StepType::ADAPTIVE:
            return getAdaptiveGradientStep(x, y, grad_x, grad_y, grad_norm);

        default:
            return m_inputData->constant_step_size;
        }
    }

    // Адаптивный шаг для градиентного метода изначальный
    /*
    double getAdaptiveGradientStep(double x, double y, double grad_x, double grad_y, double grad_norm) {
        double direction = (m_inputData->extremum_type == ExtremumType::MINIMUM) ? -1.0 : 1.0;
        double initial_step = m_inputData->constant_step_size;
        double step = initial_step;

        double current_value = evaluateFunction(x, y);
        double best_step = step;
        double best_value = current_value;

        // Пробуем разные шаги для направления градиента
        for (int i = 0; i < 10; i++) {
            double x_new = x + direction * step * grad_x;
            double y_new = y + direction * step * grad_y;

            // Проверяем границы
            x_new = std::max(m_inputData->x_left_bound, std::min(m_inputData->x_right_bound, x_new));
            y_new = std::max(m_inputData->y_left_bound, std::min(m_inputData->y_right_bound, y_new));

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
                best_step = step;
            }

            step *= STEP_REDUCTION;
            if (step < MIN_STEP) break;
        }

        return best_step;
    }
    */

    // Адаптивный шаг для градиентного метода Backtracking line search c условием Армижо
    double getAdaptiveGradientStep(double x, double y,
                               double grad_x, double grad_y,
                               double grad_norm)
    {
        double direction = (m_inputData->extremum_type == ExtremumType::MINIMUM) ? -1.0 : 1.0;

        // Стартовый шаг
        double step = m_inputData->constant_step_size;

        // Коэффициент уменьшения шага (как STEP_REDUCTION)
        const double beta = 0.5;        // Можно оставить STEP_REDUCTION, если хочешь
        const double c = 0.1;           // Параметр Армижо (обычно 0.1...0.2)

        double f_current = evaluateFunction(x, y);

        // Направление перемещения
        double dir_x = direction * grad_x;
        double dir_y = direction * grad_y;

        while (step >= MIN_STEP)
        {
            // Предлагаемая новая точка
            double x_new = x + step * dir_x;
            double y_new = y + step * dir_y;

            // Учёт границ — как у тебя
            x_new = std::max(m_inputData->x_left_bound,
                             std::min(m_inputData->x_right_bound, x_new));
            y_new = std::max(m_inputData->y_left_bound,
                             std::min(m_inputData->y_right_bound, y_new));

            // Если всё равно за границами — уменьшаем шаг
            if (!isWithinBounds(x_new, y_new)) {
                step *= beta;
                continue;
            }

            // Вычисляем новое значение функции
            double f_new = evaluateFunction(x_new, y_new);

            // Условие Армижо:
            // f_new <= f_current + c * step * grad^T * direction*grad
            double armijo_rhs =
                f_current + c * step * (grad_x * dir_x + grad_y * dir_y);

            bool armijo_ok =
                (m_inputData->extremum_type == ExtremumType::MINIMUM)
                ? (f_new <= armijo_rhs)
                : (f_new >= armijo_rhs);

            if (armijo_ok) {
                return step;        // Шаг найден!
            }

            // Иначе уменьшаем шаг
            step *= beta;
        }

        // Если всё слишком плохо — возвращаем минимальный шаг
        return MIN_STEP;
    }


    // Поиск оптимального шага вдоль направления градиента методом золотого сечения
    double findOptimalStepAlongGradient(double x, double y, double grad_x, double grad_y) {
        const double golden_ratio = 0.618033988749895;
        const double tolerance = 1e-8;
        const int max_iterations = 50;

        double direction = (m_inputData->extremum_type == ExtremumType::MINIMUM) ? -1.0 : 1.0;

        // Определяем начальный интервал для шага
        double a = 0.0;
        double b = findInitialStepBoundForGradient(x, y, grad_x, grad_y);

        if (b <= a) {
            return m_inputData->constant_step_size; // fallback
        }

        double h1 = b - (b - a) * golden_ratio;
        double h2 = a + (b - a) * golden_ratio;

        double f1 = evaluateFunctionAlongGradient(x, y, grad_x, grad_y, h1, direction);
        double f2 = evaluateFunctionAlongGradient(x, y, grad_x, grad_y, h2, direction);

        for (int i = 0; i < max_iterations && (b - a) > tolerance; ++i) {
            if (m_inputData->extremum_type == ExtremumType::MINIMUM) {
                if (f1 < f2) {
                    b = h2;
                    h2 = h1;
                    f2 = f1;
                    h1 = b - (b - a) * golden_ratio;
                    f1 = evaluateFunctionAlongGradient(x, y, grad_x, grad_y, h1, direction);
                }
                else {
                    a = h1;
                    h1 = h2;
                    f1 = f2;
                    h2 = a + (b - a) * golden_ratio;
                    f2 = evaluateFunctionAlongGradient(x, y, grad_x, grad_y, h2, direction);
                }
            }
            else { // Для максимума
                if (f1 > f2) {
                    b = h2;
                    h2 = h1;
                    f2 = f1;
                    h1 = b - (b - a) * golden_ratio;
                    f1 = evaluateFunctionAlongGradient(x, y, grad_x, grad_y, h1, direction);
                }
                else {
                    a = h1;
                    h1 = h2;
                    f1 = f2;
                    h2 = a + (b - a) * golden_ratio;
                    f2 = evaluateFunctionAlongGradient(x, y, grad_x, grad_y, h2, direction);
                }
            }
        }

        return (a + b) / 2.0;
    }

    // Вычисление функции вдоль направления градиента
    double evaluateFunctionAlongGradient(double x, double y, double grad_x, double grad_y,
        double step, double direction) {
        double x_new = x + direction * step * grad_x;
        double y_new = y + direction * step * grad_y;

        // Проверяем границы
        x_new = std::max(m_inputData->x_left_bound, std::min(m_inputData->x_right_bound, x_new));
        y_new = std::max(m_inputData->y_left_bound, std::min(m_inputData->y_right_bound, y_new));

        return evaluateFunction(x_new, y_new);
    }

    // Нахождение начальной границы для шага в градиентном методе
    double findInitialStepBoundForGradient(double x, double y, double grad_x, double grad_y) {
        double max_step = 1.0;
        double direction = (m_inputData->extremum_type == ExtremumType::MINIMUM) ? -1.0 : 1.0;

        // Адаптивно подбираем максимальный шаг на основе градиента
        double grad_norm = std::sqrt(grad_x * grad_x + grad_y * grad_y);
        if (grad_norm > 1e-10) {
            max_step = std::min(1.0, 0.5 / grad_norm);
        }

        // Убеждаемся, что шаг не выходит за границы
        int safety_counter = 0;
        while (max_step > 1e-10 && safety_counter < 20) {
            double test_x = x + direction * max_step * grad_x;
            double test_y = y + direction * max_step * grad_y;

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

    // Новая функция для поиска оптимального шага вдоль произвольного направления
    double findOptimalStepAlongDirection(double x, double y, double dir_x, double dir_y, double direction_sign) {
        const double golden_ratio = 0.618033988749895;
        const double tolerance = 1e-8;
        const int max_iterations = 50;

        // Определяем начальный интервал для шага
        double a = 0.0;
        double b = findInitialStepBoundForDirection(x, y, dir_x, dir_y);

        if (b <= a) {
            return m_inputData->constant_step_size; // fallback на дефолтный шаг
        }

        double h1 = b - (b - a) * golden_ratio;
        double h2 = a + (b - a) * golden_ratio;

        double f1 = evaluateFunctionAlongDirection(x, y, dir_x, dir_y, h1, direction_sign);
        double f2 = evaluateFunctionAlongDirection(x, y, dir_x, dir_y, h2, direction_sign);

        for (int i = 0; i < max_iterations && (b - a) > tolerance; ++i) {
            if (m_inputData->extremum_type == ExtremumType::MINIMUM) {
                if (f1 < f2) {
                    b = h2;
                    h2 = h1;
                    f2 = f1;
                    h1 = b - (b - a) * golden_ratio;
                    f1 = evaluateFunctionAlongDirection(x, y, dir_x, dir_y, h1, direction_sign);
                }
                else {
                    a = h1;
                    h1 = h2;
                    f1 = f2;
                    h2 = a + (b - a) * golden_ratio;
                    f2 = evaluateFunctionAlongDirection(x, y, dir_x, dir_y, h2, direction_sign);
                }
            }
            else { // Для максимума
                if (f1 > f2) {
                    b = h2;
                    h2 = h1;
                    f2 = f1;
                    h1 = b - (b - a) * golden_ratio;
                    f1 = evaluateFunctionAlongDirection(x, y, dir_x, dir_y, h1, direction_sign);
                }
                else {
                    a = h1;
                    h1 = h2;
                    f1 = f2;
                    h2 = a + (b - a) * golden_ratio;
                    f2 = evaluateFunctionAlongDirection(x, y, dir_x, dir_y, h2, direction_sign);
                }
            }
        }

        return (a + b) / 2.0;
    }

    // Вычисление функции вдоль произвольного направления
    double evaluateFunctionAlongDirection(double x, double y, double dir_x, double dir_y,
        double step, double direction_sign) {
        double x_new = x + direction_sign * step * dir_x;
        double y_new = y + direction_sign * step * dir_y;

        // Проверяем границы
        x_new = std::max(m_inputData->x_left_bound, std::min(m_inputData->x_right_bound, x_new));
        y_new = std::max(m_inputData->y_left_bound, std::min(m_inputData->y_right_bound, y_new));

        return evaluateFunction(x_new, y_new);
    }

    // Нахождение начальной границы для шага вдоль произвольного направления
    double findInitialStepBoundForDirection(double x, double y, double dir_x, double dir_y) {
        double max_step = 1.0;
        double direction_sign = (m_inputData->extremum_type == ExtremumType::MINIMUM) ? -1.0 : 1.0;

        // Адаптивно подбираем максимальный шаг на основе нормализованного направления
        double dir_norm = std::sqrt(dir_x * dir_x + dir_y * dir_y);
        if (dir_norm > 1e-10) {
            max_step = std::min(1.0, 0.5 / dir_norm);
        }

        // Убеждаемся, что шаг не выходит за границы
        int safety_counter = 0;
        while (max_step > 1e-10 && safety_counter < 20) {
            double test_x = x + direction_sign * max_step * dir_x;
            double test_y = y + direction_sign * max_step * dir_y;

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

    // Улучшенная детекция оврагов
    double detectRavineFactor(const std::vector<std::pair<double, double>>& gradients,
        const std::vector<std::pair<double, double>>& trajectory) {
        if (gradients.size() < 3) return 0.0;

        double factor = 0.0;

        // 1. Колебания градиента (основной признак)
        double angle_variance = computeGradientAngleVariance(gradients);
        factor += angle_variance * 0.6;

        // 2. Вытянутость траектории
        if (trajectory.size() >= 3) {
            double elongation = computeTrajectoryElongation(trajectory);
            factor += elongation * 0.4;
        }

        return std::min(1.0, factor);
    }

    // Вычисление дисперсии углов градиента
    double computeGradientAngleVariance(const std::vector<std::pair<double, double>>& gradients) {
        if (gradients.size() < 2) return 0.0;

        std::vector<double> angles;
        for (size_t i = 1; i < gradients.size(); ++i) {
            double dot = gradients[i - 1].first * gradients[i].first +
                gradients[i - 1].second * gradients[i].second;
            double norm1 = std::sqrt(gradients[i - 1].first * gradients[i - 1].first +
                gradients[i - 1].second * gradients[i - 1].second);
            double norm2 = std::sqrt(gradients[i].first * gradients[i].first +
                gradients[i].second * gradients[i].second);

            if (norm1 > 1e-10 && norm2 > 1e-10) {
                double cos_angle = dot / (norm1 * norm2);
                cos_angle = std::max(-1.0, std::min(1.0, cos_angle)); // Обеспечиваем корректный диапазон
                double angle = std::acos(cos_angle);
                angles.push_back(angle);
            }
        }

        if (angles.empty()) return 0.0;

        // Вычисляем дисперсию углов
        double mean = 0.0;
        for (double angle : angles) mean += angle;
        mean /= angles.size();

        double variance = 0.0;
        for (double angle : angles) {
            variance += (angle - mean) * (angle - mean);
        }
        variance /= angles.size();

        // Нормализуем: большая дисперсия = сильные колебания = овраг
        return std::min(1.0, variance / (M_PI * M_PI)); // M_PI^2 - максимальная возможная дисперсия
    }

    // Вычисление вытянутости траектории
    double computeTrajectoryElongation(const std::vector<std::pair<double, double>>& trajectory) {
        if (trajectory.size() < 2) return 0.0;

        // Вычисляем длину траектории и прямое расстояние
        double path_length = 0.0;
        for (size_t i = 1; i < trajectory.size(); ++i) {
            double dx = trajectory[i].first - trajectory[i - 1].first;
            double dy = trajectory[i].second - trajectory[i - 1].second;
            path_length += std::sqrt(dx * dx + dy * dy);
        }

        double direct_distance = std::sqrt(
            std::pow(trajectory.back().first - trajectory.front().first, 2) +
            std::pow(trajectory.back().second - trajectory.front().second, 2)
        );

        if (direct_distance < 1e-10) return 1.0; // Траектория замкнута - вероятно овраг

        // Коэффициент вытянутости: 1 - прямая линия, >1 - извилистая
        double elongation = path_length / direct_distance;

        // Преобразуем в фактор оврага: чем больше elongation, тем больше вероятность оврага
        return std::min(1.0, (elongation - 1.0) / 2.0); // Нормализуем к [0,1]
    }

    // Комбинированное направление (градиент + направление оврага)
    void computeCombinedDirection(double grad_x, double grad_y,
        const std::vector<std::pair<double, double>>& trajectory,
        double ravine_factor, double& dir_x, double& dir_y) {
        // Нормализуем градиент
        double grad_norm = std::sqrt(grad_x * grad_x + grad_y * grad_y);
        double grad_dir_x = grad_x / grad_norm;
        double grad_dir_y = grad_y / grad_norm;

        // Вычисляем направление оврага
        double ravine_dir_x, ravine_dir_y;
        computeRavineDirection(trajectory, ravine_dir_x, ravine_dir_y);

        // Комбинируем направления с весом, зависящим от силы оврага
        double weight = ravine_factor; // 0.1-0.3 -> вес 10%-30%

        dir_x = (1.0 - weight) * grad_dir_x + weight * ravine_dir_x;
        dir_y = (1.0 - weight) * grad_dir_y + weight * ravine_dir_y;

        // Нормализуем результат
        double norm = std::sqrt(dir_x * dir_x + dir_y * dir_y);
        if (norm > 1e-10) {
            dir_x /= norm;
            dir_y /= norm;
        }
        else {
            dir_x = grad_dir_x; dir_y = grad_dir_y; // fallback
        }
    }

    // Вычисление направления вдоль оврага
    void computeRavineDirection(const std::vector<std::pair<double, double>>& trajectory,
        double& dir_x, double& dir_y) {
        if (trajectory.size() < 2) {
            dir_x = 1.0; dir_y = 0.0; // fallback
            return;
        }

        // Направление - ось главной компоненты траектории
        size_t n = trajectory.size();
        double mean_x = 0, mean_y = 0;

        for (const auto& point : trajectory) {
            mean_x += point.first;
            mean_y += point.second;
        }
        mean_x /= n; mean_y /= n;

        // Ковариационная матрица
        double cov_xx = 0, cov_xy = 0, cov_yy = 0;
        for (const auto& point : trajectory) {
            double dx = point.first - mean_x;
            double dy = point.second - mean_y;
            cov_xx += dx * dx;
            cov_xy += dx * dy;
            cov_yy += dy * dy;
        }

        // Собственный вектор для наибольшего собственного значения
        double trace = cov_xx + cov_yy;
        double determinant = cov_xx * cov_yy - cov_xy * cov_xy;
        double eigenvalue = (trace + std::sqrt(trace * trace - 4 * determinant)) / 2;

        dir_x = cov_xy;
        dir_y = eigenvalue - cov_xx;

        // Нормализация
        double norm = std::sqrt(dir_x * dir_x + dir_y * dir_y);
        if (norm > 1e-10) {
            dir_x /= norm;
            dir_y /= norm;
        }
    }

    void ReporterResult(double best_x, double best_y, double best_f, int m_function_calls, int m_iterations) {
        m_reporter->insertMessage("Итог:");
        m_reporter->insertMessage("Количество итераций: " + std::to_string(m_iterations));
        m_reporter->insertMessage("Количество вызовов функции: " + std::to_string(m_function_calls));
        m_reporter->insertResult(best_x, best_y, best_f);
    }

    //Округляет число до указанного количества знаков после запятой
    double roundTo(const double value_1, const int digits) {
        double factor = pow(10.0, digits);
        return round(value_1 * factor) / factor;
    }

    inline double roundComputation(double v)
    {
        double factor = std::pow(10.0, m_digitComputationPrecision);
        return std::round(v * factor) / factor;
    }

    inline double roundResult(double v)
    {
        double factor = std::pow(10.0, m_digitResultPrecision);
        return std::round(v * factor) / factor;
    }
};

} // namespace GD

#endif // GRADIENTDESCENT_GRADIENTDESCENT_HPP_
