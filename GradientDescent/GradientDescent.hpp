//
// Created on 25 Oct, 2025
//  by alecproj
//

#ifndef GRADIENTDESCENT_GRADIENTDESCENT_HPP_
#define GRADIENTDESCENT_GRADIENTDESCENT_HPP_

#include <GradientDescent/Common.hpp>
#include <chrono>
#include <functional>
#include <regex>
#include <set>
#include <stdexcept>

#include "muParser.h"

template <typename Reporter>
class GradientDescent {

    // Пример объявления и инициализации констант
    static constexpr auto CONST_VALUE_EXAMLE{0};

public:

    explicit GradientDescent(Reporter *reporter) :
        m_inputData{nullptr},
        m_reporter{reporter}
    {
    }

    GDResult setInputData(const GradientInput *data)
{
    // === 1. Проверка указателя ===
    if (!data) {
        std::cerr << "Ошибка: передан nullptr вместо данных.\n";
        return GDResult::InvalidInput;
    }

    // === 2. Проверка строки функции ===
    if (data->function.empty()) {
        std::cerr << "Ошибка: поле 'function' пустое.\n";
        return GDResult::InvalidInput;
    }

    if (!validateFunctionVariables(data->function)) {
        std::cerr << "Ошибка: функция содержит недопустимые идентификаторы.\n";
        std::cerr << "Разрешены только переменные x, y и функции: sin, cos, exp, sqrt, pow\n";
        return GDResult::InvalidInput;
    }

    // === 3. Проверки числовых параметров ===

    // Стартовая точка
    if (std::isnan(data->initialApproximationX_0) ||
        std::isnan(data->initialApproximationY_0))
    {
        std::cerr << "Ошибка: стартовые координаты содержат NaN.\n";
        return GDResult::InvalidInput;
    }

    // Границы области
    if (data->leftBorderX >= data->rightBorderX) {
        std::cerr << "Ошибка: левая граница X >= правой.\n";
        return GDResult::InvalidInput;
    }
    if (data->leftBorderY >= data->rightBorderY) {
        std::cerr << "Ошибка: левая граница Y >= правой.\n";
        return GDResult::InvalidInput;
    }

    // Начальная точка лежит в области
    if (data->initialApproximationX_0 < data->leftBorderX ||
        data->initialApproximationX_0 > data->rightBorderX ||
        data->initialApproximationY_0 < data->leftBorderY ||
        data->initialApproximationY_0 > data->rightBorderY)
    {
        std::cerr << "Ошибка: стартовая точка вне допустимой области.\n";
        return GDResult::InvalidInput;
    }

    // Шаг и точности
    if (data->coefficientStep <= 0) {
        std::cerr << "Ошибка: коэффициентный шаг должен быть > 0.\n";
        return GDResult::InvalidInput;
    }

    if (data->resultAccuracy <= 0) {
        std::cerr << "Ошибка: точность результата (eps) должна быть > 0.\n";
        return GDResult::InvalidInput;
    }

    if (data->calculationAccuracy <= 0) {
        std::cerr << "Ошибка: точность вычислений (calc_eps) должна быть > 0.\n";
        return GDResult::InvalidInput;
    }

    // Предупреждение, если шаг слишком маленький
    if (data->calculationAccuracy < 1e-12) {
        std::cerr << "Предупреждение: слишком маленькая точность вычислений (calc_eps < 1e-12). Возможны численные ошибки.\n";
    }

    if (data->extended < 0 || data->extended > 2) {
        std::cerr << "Ошибка: неверный выбор расширения!\n";
        return GDResult::InvalidInput;
    }
    if (data->minOrMax == -1) {
        std::cerr << "Ошибка: не выбран поиск min или max осуществляется.\n";
        return GDResult::InvalidInput;
    }

    // === 4. Проверка функции на вычислимость + окрестность ===
    try {
        mu::Parser p;
        double x = data->initialApproximationX_0;
        double y = data->initialApproximationY_0;
        double d = data->calculationAccuracy;

        p.DefineVar("x", &x);
        p.DefineVar("y", &y);
        p.SetExpr(data->function);

        auto safeEval = [&](double X, double Y) -> bool {
            try {
                x = X;
                y = Y;
                volatile double val = p.Eval();
                return std::isfinite(val);
            } catch (...) {
                return false;
            }
        };

        // В стартовой точке
        if (!safeEval(x, y)) {
            std::cerr << "Ошибка: функция не вычисляется в стартовой точке ("
                      << x << ", " << y << ").\n";
            return GDResult::InvalidInput;
        }

        // Перед проверкой окрестности убедимся, что окрестность лежит в границах
        if ( (x + d > data->rightBorderX) || (x - d < data->leftBorderX) ||
             (y + d > data->rightBorderY) || (y - d < data->leftBorderY) ) {
            std::cerr << "Ошибка: окрестность для численного градиента (x +/- d, y +/- d) выходит за границы области.\n";
            return GDResult::InvalidInput;
             }

        // Дифференцируемость через окрестность
        if (!safeEval(x + d, y) || !safeEval(x - d, y) ||
            !safeEval(x, y + d) || !safeEval(x, y - d))
        {
            std::cerr << "Ошибка: функция не вычисляется в малой окрестности стартовой точки.\n"
                      << "Проверьте формулу или границы области.\n";
            return GDResult::InvalidInput;
        }

    } catch (mu::Parser::exception_type &e) {
        std::cerr << "Ошибка muParser: " << e.GetMsg() << "\n";
        return GDResult::InvalidInput;
    }

    // === 5. Данные валидны ===
    m_inputData = data;
    return GDResult::Success;
}
    GDResult solve()
    {
        if (!m_inputData) return GDResult::Fail;
        // === Проверки однозначные перед вычислениями ===
        if (m_inputData->coefficientStep < 1e-12) {
            std::cerr << "[WARN] Коэффициентный шаг h слишком мал (" << m_inputData->coefficientStep << "). Возможны проблемы с сходимостью.\n";
        }
        if (m_inputData->resultAccuracy < 1e-12) {
            std::cerr << "[WARN] Точность результата eps слишком мала (" << m_inputData->resultAccuracy << "). Возможны численные ошибки.\n";
        }
        if (m_inputData->calculationAccuracy < 1e-12) {
            std::cerr << "[WARN] Точность вычислений delta слишком мала (" << m_inputData->calculationAccuracy << "). Возможны численные ошибки.\n";
        }
        if (m_inputData->initialApproximationX_0 <= m_inputData->leftBorderX + 1e-14 ||
            m_inputData->initialApproximationX_0 >= m_inputData->rightBorderX - 1e-14 ||
            m_inputData->initialApproximationY_0 <= m_inputData->leftBorderY + 1e-14 ||
            m_inputData->initialApproximationY_0 >= m_inputData->rightBorderY - 1e-14)
        {
            std::cerr << "[WARN] Начальная точка находится на границе области или очень близко к ней: "
                      << "(" << m_inputData->initialApproximationX_0 << ", " << m_inputData->initialApproximationY_0 << ")\n";
        }

if (m_inputData->extended == 0) {
    try {
        mu::Parser p;
        double x = m_inputData->initialApproximationX_0;
        double y = m_inputData->initialApproximationY_0;
        p.DefineVar("x", &x);
        p.DefineVar("y", &y);
        p.SetExpr(m_inputData->function);

        double h = m_inputData->coefficientStep; // начальный шаг
        constexpr double h_min = 1e-6;           // минимальный шаг
        constexpr double h_decay = 0.9;          // коэффициент уменьшения шага
        const double eps = m_inputData->resultAccuracy;
        const double delta = m_inputData->calculationAccuracy;

        auto evalFunc = [&](double X, double Y) -> double {
            double old_x = x, old_y = y;
            x = X; y = Y;
            double val = p.Eval();
            x = old_x; y = old_y;
            return val;
        };

        auto start_time = std::chrono::steady_clock::now();
        constexpr double max_seconds = 60.0;
        int iter = 0;

        while (true) {
            auto current_time = std::chrono::steady_clock::now();
            double elapsed = std::chrono::duration<double>(current_time - start_time).count();
            if (elapsed >= max_seconds) {
                std::cerr << "[ERROR] Метод не сошелся за " << max_seconds << " секунд.\n";
                return GDResult::Fail;
            }

            // Численный градиент
            double fx = (evalFunc(x + delta, y) - evalFunc(x - delta, y)) / (2.0 * delta);
            double fy = (evalFunc(x, y + delta) - evalFunc(x, y - delta)) / (2.0 * delta);
            double grad_norm = std::sqrt(fx*fx + fy*fy);

            if (!std::isfinite(grad_norm)) {
                std::cerr << "[ERROR] Градиент нечисловой (NaN или Inf) на итерации " << iter << ".\n";
                return GDResult::Fail;
            }

            double f_val = evalFunc(x, y);
            std::cout << "[DEBUG] n=" << iter
                      << ", x=" << x
                      << ", y=" << y
                      << ", f=" << f_val
                      << ", df/dx=" << fx
                      << ", df/dy=" << fy
                      << ", |grad|=" << grad_norm
                      << ", h=" << h << "\n";

            // --- Критерий остановки по градиенту ---
            if (grad_norm < eps) {
                std::cout << "[DEBUG] Модуль градиента < eps, выход из цикла.\n";
                return GDResult::Success;
            }

            // --- Локальный цикл для адаптивного шага ---
            double h_local = h;
            double x_new, y_new, f_new;
            while (true) {
                if (m_inputData->minOrMax == 0) { // минимум
                    x_new = x - h_local * fx;
                    y_new = y - h_local * fy;
                } else { // максимум
                    x_new = x + h_local * fx;
                    y_new = y + h_local * fy;
                }

                // Ограничение по границам
                x_new = std::min(std::max(x_new, m_inputData->leftBorderX), m_inputData->rightBorderX);
                y_new = std::min(std::max(y_new, m_inputData->leftBorderY), m_inputData->rightBorderY);

                f_new = evalFunc(x_new, y_new);

                bool improve = (m_inputData->minOrMax == 0) ? (f_new < f_val) : (f_new > f_val);

                // Если шаг минимальный и улучшения нет — выходим
                if (improve || h_local <= h_min) break;

                h_local *= h_decay;
            }

            // --- Проверка изменения функции и координат для остановки ---
            if (std::abs(f_new - f_val) < delta &&
                std::abs(x_new - x) < 1e-8 && std::abs(y_new - y) < 1e-8) {
                std::cout << "[DEBUG] Изменение функции и координат очень мало, выход из цикла.\n";
                return GDResult::Success;
            }

            // Применяем обновление
            x = x_new;
            y = y_new;
            h = h_local;

            iter++;
        }
    }
    catch (mu::Parser::exception_type &e) {
        std::cerr << "Ошибка muParser: " << e.GetMsg() << "\n";
        return GDResult::Fail;
    }
    catch (...) {
        std::cerr << "Неизвестная ошибка в GradientDescent::solve\n";
        return GDResult::Fail;
    }
}

    // ====== Метод наискорейшего спуска ======
if (m_inputData->extended == 1) {
    try {
        mu::Parser p;
        double x = m_inputData->initialApproximationX_0;
        double y = m_inputData->initialApproximationY_0;
        p.DefineVar("x", &x);
        p.DefineVar("y", &y);
        p.SetExpr(m_inputData->function);

        const double eps = m_inputData->resultAccuracy;
        const double delta = std::max(m_inputData->calculationAccuracy, 1e-8); // минимальная точность
        constexpr double max_step = 1.0;
        constexpr double max_seconds = 100.0;

        auto evalFunc = [&](double X, double Y) -> double {
            x = X; y = Y;
            return p.Eval();
        };

        auto start_time = std::chrono::steady_clock::now();
        int iter = 0;

        while (true) {
            auto current_time = std::chrono::steady_clock::now();
            double elapsed = std::chrono::duration<double>(current_time - start_time).count();
            if (elapsed >= max_seconds) {
                std::cerr << "[ERROR] Метод не сошелся за " << max_seconds << " секунд.\n";
                return GDResult::Fail;
            }

            // численный градиент
            double fx = (evalFunc(x + delta, y) - evalFunc(x - delta, y)) / (2.0 * delta);
            double fy = (evalFunc(x, y + delta) - evalFunc(x, y - delta)) / (2.0 * delta);

            // направление градиента для минимума или максимума
            double dirX = (m_inputData->minOrMax == 0) ? -fx : fx;
            double dirY = (m_inputData->minOrMax == 0) ? -fy : fy;

            double grad_norm = std::sqrt(dirX*dirX + dirY*dirY);

            // проверка завершения
            if (grad_norm < eps) {
                std::cout << "[INFO] Модуль градиента < eps, выход из метода.\n";
                break;
            }

            // нормализация направления
            dirX /= grad_norm;
            dirY /= grad_norm;

            // безопасный стартовый шаг для метода Пауэлла
            double h0 = std::min(0.1, 1.0 / grad_norm);

            // функция вдоль направления
            auto phi = [&](double h_val) -> double {
                return evalFunc(x + h_val * dirX, y + h_val * dirY);
            };

            // поиск оптимального шага методом Пауэлла
            double h_opt = powell_method(phi, h0, delta, delta, 0.0, max_step, max_seconds);

            if (!std::isfinite(h_opt) || std::fabs(h_opt) < 1e-12) {
                h_opt = std::min(0.01, h0); // минимальный безопасный шаг
            }

            // обновление координат
            x += h_opt * dirX;
            y += h_opt * dirY;

            // ограничение по границам
            x = std::min(std::max(x, m_inputData->leftBorderX), m_inputData->rightBorderX);
            y = std::min(std::max(y, m_inputData->leftBorderY), m_inputData->rightBorderY);

            // дебаг
            double f_val = evalFunc(x, y);
            std::cout << "[DEBUG] iter=" << iter
                      << ", x=" << x << ", y=" << y
                      << ", f(x,y)=" << f_val
                      << ", |grad|=" << grad_norm
                      << ", h_opt=" << h_opt << "\n";

            iter++;
        }

        std::cout << "[RESULT] x=" << x << ", y=" << y
                  << ", f(x,y)=" << evalFunc(x, y) << "\n";

    } catch (mu::Parser::exception_type &e) {
        std::cerr << "[ERROR] Ошибка muParser в методе наискорейшего спуска: " << e.GetMsg() << "\n";
        return GDResult::Fail;
    } catch (...) {
        std::cerr << "[ERROR] Неизвестная ошибка в методе наискорейшего спуска.\n";
        return GDResult::Fail;
    }
}




if (m_inputData && m_inputData->extended == 2) {
    try {
        mu::Parser p;
        double x1 = m_inputData->initialApproximationX_0;
        double y1 = m_inputData->initialApproximationY_0;
        double x2 = x1;
        double y2 = y1;

        p.DefineVar("x", &x1);
        p.DefineVar("y", &y1);
        p.SetExpr(m_inputData->function);

        const double eps = m_inputData->resultAccuracy;
        const double delta = m_inputData->calculationAccuracy;
        const double h = m_inputData->coefficientStep;
        double d = 1.0; // начальный овражный шаг

        auto evalFunc = [&](double X, double Y) -> double {
            double old_x = x1, old_y = y1;
            x1 = X; y1 = Y;
            double val = p.Eval();
            x1 = old_x; y1 = old_y;
            return val;
        };

        // === 1. Инициализация второй точки ===
        x2 = x1 + std::max(delta, 1e-2);
        y2 = y1 + std::max(delta, 1e-2);

        // Предупреждение, если точки на границе
        if ((x1 <= m_inputData->leftBorderX + 1e-14 || x1 >= m_inputData->rightBorderX - 1e-14 ||
             y1 <= m_inputData->leftBorderY + 1e-14 || y1 >= m_inputData->rightBorderY - 1e-14) &&
            (x2 <= m_inputData->leftBorderX + 1e-14 || x2 >= m_inputData->rightBorderX - 1e-14 ||
             y2 <= m_inputData->leftBorderY + 1e-14 || y2 >= m_inputData->rightBorderY - 1e-14)) {
            std::cerr << "[WARN] Обе точки овражного метода находятся на границе или очень близко к Sald.\n";
        }

        auto start_time = std::chrono::steady_clock::now();
        int iter = 0;
        constexpr double max_seconds = 60.0;

        while (true) {
            auto current_time = std::chrono::steady_clock::now();
            double elapsed = std::chrono::duration<double>(current_time - start_time).count();
            if (elapsed >= max_seconds) {
                std::cerr << "[ERROR] Метод не сошелся за " << max_seconds << " секунд.\n";
                return GDResult::Fail;
            }

            // --- Численные градиенты ---
            double fx1 = (evalFunc(x1 + delta, y1) - evalFunc(x1 - delta, y1)) / (2.0 * delta);
            double fy1 = (evalFunc(x1, y1 + delta) - evalFunc(x1, y1 - delta)) / (2.0 * delta);
            double fx2 = (evalFunc(x2 + delta, y2) - evalFunc(x2 - delta, y2)) / (2.0 * delta);
            double fy2 = (evalFunc(x2, y2 + delta) - evalFunc(x2, y2 - delta)) / (2.0 * delta);

            // --- DEBUG: вывод текущих значений ---
            double f1_val = evalFunc(x1, y1);
            double f2_val = evalFunc(x2, y2);
            std::cout << "[DEBUG] iter=" << iter
                      << ", x1=" << x1 << ", y1=" << y1 << ", f1=" << f1_val
                      << ", df1/dx=" << fx1 << ", df1/dy=" << fy1
                      << ", x2=" << x2 << ", y2=" << y2 << ", f2=" << f2_val
                      << ", df2/dx=" << fx2 << ", df2/dy=" << fy2 << "\n";

            // --- Обновление координат ---
            if (m_inputData->minOrMax == 0) {
                x1 -= h * fx1; y1 -= h * fy1;
                x2 -= h * fx2; y2 -= h * fy2;
            } else {
                x1 += h * fx1; y1 += h * fy1;
                x2 += h * fx2; y2 += h * fy2;
            }

            // Ограничение по границам
            x1 = std::min(std::max(x1, m_inputData->leftBorderX), m_inputData->rightBorderX);
            y1 = std::min(std::max(y1, m_inputData->leftBorderY), m_inputData->rightBorderY);
            x2 = std::min(std::max(x2, m_inputData->leftBorderX), m_inputData->rightBorderX);
            y2 = std::min(std::max(y2, m_inputData->leftBorderY), m_inputData->rightBorderY);

            // --- Овражный шаг для минимума ---
            if (m_inputData->minOrMax == 0 && f2_val < f1_val) {
                double x_next = x1 + std::max(d * (x2 - x1), 1e-3);
                double y_next = y1 + std::max(d * (y2 - y1), 1e-3);
                double f_next = evalFunc(x_next, y_next);
                if (f_next > f2_val)
                    d = std::max(d / 2.0, 1e-3);
                else {
                    x1 = x_next;
                    y1 = y_next;
                }
            }

            // --- Овражный шаг для максимума ---
            if (m_inputData->minOrMax == 1 && f2_val > f1_val) {
                double x_next = x1 + std::max(d * (x2 - x1), 1e-3);
                double y_next = y1 + std::max(d * (y2 - y1), 1e-3);
                double f_next = evalFunc(x_next, y_next);
                if (f_next < f2_val)
                    d = std::max(d / 2.0, 1e-3);
                else {
                    x1 = x_next;
                    y1 = y_next;
                }
            }

            // --- Критерий остановки ---
            double grad_norm = std::sqrt(fx1*fx1 + fy1*fy1);
            if (grad_norm < eps) {
                std::cout << "[INFO] Модуль градиента < eps, завершение овражного метода.\n";
                break;
            }

            iter++;
        }

        if (m_inputData->minOrMax == 0) {
            std::cout << "[RESULT] Минимум найден: x=" << x1 << ", y=" << y1
                      << ", f(x,y)=" << evalFunc(x1, y1) << "\n";
        } else {
            std::cout << "[RESULT] Максимум найден: x=" << x1 << ", y=" << y1
                      << ", f(x,y)=" << evalFunc(x1, y1) << "\n";
        }

    } catch (mu::Parser::exception_type &e) {
        std::cerr << "[ERROR] Ошибка muParser в овражном методе: " << e.GetMsg() << "\n";
        return GDResult::Fail;
    } catch (...) {
        std::cerr << "[ERROR] Неизвестная ошибка в овражном методе.\n";
        return GDResult::Fail;
    }
}



        if (m_reporter->end() == 0) return GDResult::Success;
        else return GDResult::Fail;
    }


private:

    const GradientInput *m_inputData;
    Reporter *m_reporter;

    static bool validateFunctionVariables(const std::string &funcStr) {
        // Разрешённые имена функций и переменные
        const std::set<std::string> allowedIds = {"x", "y", "sin", "cos", "exp", "sqrt", "pow","_pi", "_e"};

        // Токены: последовательности букв/цифр/нижнего подчеркивания, начинающиеся с буквы
        std::regex tokenPattern(R"(\b[a-zA-Z_]\w*\b)");
        std::smatch match;
        std::string s = funcStr;

        while (std::regex_search(s, match, tokenPattern)) {
            std::string token = match.str();

            // Если слово не из разрешенного списка — ошибка
            if (!allowedIds.contains(token)) {
                std::cerr << "[ERROR] Недопустимый идентификатор в функции: " << token << std::endl;
                return false;
            }

            s = match.suffix().str();
        }

        return true;
    }
    static double powell_method(const std::function<double(double)>& func,
                            double x0,
                            double delta,
                            double tol,
                            double left_bound,
                            double right_bound,
                            const double max_seconds = 60.0)
{
    if (!func) throw std::invalid_argument("func is nullptr");
    if (delta <= 0) throw std::invalid_argument("delta <= 0");
    if (tol <= 0) throw std::invalid_argument("tol <= 0");
    if (left_bound >= right_bound) throw std::invalid_argument("left_bound >= right_bound");
    if (max_seconds <= 0) throw std::invalid_argument("max_seconds <= 0");

    auto start_time = std::chrono::steady_clock::now();

    double x1 = std::min(std::max(x0, left_bound), right_bound);
    double x2 = std::min(std::max(x1 + delta, left_bound), right_bound);
    double x3 = std::min(std::max(x1 - delta, left_bound), right_bound);

    double f1 = func(x1);
    double f2 = func(x2);
    double f3 = func(x3);

    if (!std::isfinite(f1)) f1 = f2 + 1.0;
    if (!std::isfinite(f2)) f2 = f1 + 1.0;
    if (!std::isfinite(f3)) f3 = f1 + 1.0;

    for (;;) {
        auto current_time = std::chrono::steady_clock::now();
        if (std::chrono::duration<double>(current_time - start_time).count() >= max_seconds)
            break;

        double numerator = (x2 - x1)*(x2 - x1)*(f2 - f3) - (x2 - x3)*(x2 - x3)*(f2 - f1);
        double denominator = 2.0 * ((x2 - x1)*(f2 - f3) - (x2 - x3)*(f2 - f1));

        double x_min;
        if (std::fabs(denominator) < 1e-12) {
            // избегаем деления на ноль, делаем маленький безопасный шаг
            double step = 0.5 * (x2 - x3);
            if (std::fabs(step) < 1e-8) step = delta * 0.1;
            x_min = x1 - step;
        } else {
            x_min = x2 - numerator / denominator;
        }

        // Ограничение по границам
        x_min = std::min(std::max(x_min, left_bound), right_bound);

        double f_min = func(x_min);
        if (!std::isfinite(f_min)) f_min = f1 + 1.0;

        if (std::fabs(x_min - x1) < tol) {
            return x_min;
        }

        // Сдвигаем точки
        x3 = x2; f3 = f2;
        x2 = x1; f2 = f1;
        x1 = x_min; f1 = f_min;
    }

    return x1;
}



};

#endif // GRADIENTDESCENT_GRADIENTDESCENT_HPP_
