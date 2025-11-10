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

        const double eps = m_inputData->resultAccuracy;
        const double delta = std::max(m_inputData->calculationAccuracy, 1e-8);
        double h_init = m_inputData->coefficientStep;
        constexpr double max_seconds = 60.0;

        auto evalFunc = [&](double X, double Y) -> double {
            x = X; y = Y;
            return p.Eval();
        };

        auto start_time = std::chrono::steady_clock::now();
        int iter = 0;

        while (iter < 10000) {
            auto current_time = std::chrono::steady_clock::now();
            if (std::chrono::duration<double>(current_time - start_time).count() > max_seconds) break;

            // численный градиент
            double fx = (evalFunc(x + delta, y) - evalFunc(x - delta, y)) / (2.0 * delta);
            double fy = (evalFunc(x, y + delta) - evalFunc(x, y - delta)) / (2.0 * delta);

            double grad_norm = std::sqrt(fx*fx + fy*fy);
            if (grad_norm < eps) break;

            // направление градиента
            double dirX = (m_inputData->minOrMax == 0) ? -fx : fx;
            double dirY = (m_inputData->minOrMax == 0) ? -fy : fy;
            dirX /= grad_norm; dirY /= grad_norm;

            // одномерная оптимизация вдоль направления (как phi и minimize_scalar)
            auto phi = [&](double h) -> double {
                double xt = x + h*dirX, yt = y + h*dirY;
                xt = std::clamp(xt, m_inputData->leftBorderX, m_inputData->rightBorderX);
                yt = std::clamp(yt, m_inputData->leftBorderY, m_inputData->rightBorderY);
                double val = evalFunc(xt, yt);
                return (m_inputData->minOrMax == 0) ? val : -val;
            };

            double h_opt = powell_method(phi, h_init, 1e-8, delta, 0.0, 10.0, max_seconds);
            x = std::clamp(x + h_opt * dirX, m_inputData->leftBorderX, m_inputData->rightBorderX);
            y = std::clamp(y + h_opt * dirY, m_inputData->leftBorderY, m_inputData->rightBorderY);

            iter++;
        }
        std::cout << "[RESULT] x=" << x << ", y=" << y << ", f=" << evalFunc(x, y) << "\n";
    } catch (...) { return GDResult::Fail; }
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
        const double delta = std::max(m_inputData->calculationAccuracy, 1e-8);
        double h0 = std::min(0.1, 1.0);

        auto evalFunc = [&](double X, double Y) -> double { x = X; y = Y; return p.Eval(); };

        int iter = 0;
        constexpr double max_seconds = 100.0;
        auto start_time = std::chrono::steady_clock::now();

        while (iter < 10000) {
            auto current_time = std::chrono::steady_clock::now();
            if (std::chrono::duration<double>(current_time - start_time).count() > max_seconds) break;

            double fx = (evalFunc(x + delta, y) - evalFunc(x - delta, y)) / (2.0 * delta);
            double fy = (evalFunc(x, y + delta) - evalFunc(x, y - delta)) / (2.0 * delta);

            double grad_norm = std::sqrt(fx*fx + fy*fy);
            if (grad_norm < eps) break;

            double dirX = (m_inputData->minOrMax == 0) ? -fx : fx;
            double dirY = (m_inputData->minOrMax == 0) ? -fy : fy;
            dirX /= grad_norm; dirY /= grad_norm;

            auto phi = [&](double h) -> double {
                double xt = x + h*dirX, yt = y + h*dirY;
                xt = std::clamp(xt, m_inputData->leftBorderX, m_inputData->rightBorderX);
                yt = std::clamp(yt, m_inputData->leftBorderY, m_inputData->rightBorderY);
                double val = evalFunc(xt, yt);
                return (m_inputData->minOrMax == 0) ? val : -val;
            };

            double h_opt = powell_method(phi, h0, 1e-8, delta, 0.0, 1.0, max_seconds);
            x = std::clamp(x + h_opt * dirX, m_inputData->leftBorderX, m_inputData->rightBorderX);
            y = std::clamp(y + h_opt * dirY, m_inputData->leftBorderY, m_inputData->rightBorderY);

            iter++;
        }
        std::cout << "[RESULT] x=" << x << ", y=" << y << ", f=" << evalFunc(x, y) << "\n";
    } catch (...) { return GDResult::Fail; }
}





if (m_inputData && m_inputData->extended == 2) {
    try {
        mu::Parser p;
        double x1 = m_inputData->initialApproximationX_0;
        double y1 = m_inputData->initialApproximationY_0;
        double x2 = x1 + std::max(m_inputData->calculationAccuracy, 1e-2);
        double y2 = y1 + std::max(m_inputData->calculationAccuracy, 1e-2);
        p.DefineVar("x", &x1);
        p.DefineVar("y", &y1);
        p.SetExpr(m_inputData->function);

        const double eps = m_inputData->resultAccuracy;
        const double delta = std::max(m_inputData->calculationAccuracy, 1e-8);
        double h = m_inputData->coefficientStep;

        auto evalFunc = [&](double X, double Y) -> double {
            double old_x = x1, old_y = y1;
            x1 = X; y1 = Y;
            double val = p.Eval();
            x1 = old_x; y1 = old_y;
            return val;
        };

        int iter = 0;
        constexpr double max_seconds = 120.0;
        auto start_time = std::chrono::steady_clock::now();

        while (iter < 10000) {
            auto current_time = std::chrono::steady_clock::now();
            if (std::chrono::duration<double>(current_time - start_time).count() > max_seconds) break;

            double fx1 = (evalFunc(x1 + delta, y1) - evalFunc(x1 - delta, y1)) / (2.0*delta);
            double fy1 = (evalFunc(x1, y1 + delta) - evalFunc(x1, y1 - delta)) / (2.0*delta);
            double fx2 = (evalFunc(x2 + delta, y2) - evalFunc(x2 - delta, y2)) / (2.0*delta);
            double fy2 = (evalFunc(x2, y2 + delta) - evalFunc(x2, y2 - delta)) / (2.0*delta);

            double x1_new = (m_inputData->minOrMax==0)? x1-h*fx1 : x1+h*fx1;
            double y1_new = (m_inputData->minOrMax==0)? y1-h*fy1 : y1+h*fy1;
            double x2_new = (m_inputData->minOrMax==0)? x2-h*fx2 : x2+h*fx2;
            double y2_new = (m_inputData->minOrMax==0)? y2-h*fy2 : y2+h*fy2;

            x1_new = std::clamp(x1_new, m_inputData->leftBorderX, m_inputData->rightBorderX);
            y1_new = std::clamp(y1_new, m_inputData->leftBorderY, m_inputData->rightBorderY);
            x2_new = std::clamp(x2_new, m_inputData->leftBorderX, m_inputData->rightBorderX);
            y2_new = std::clamp(y2_new, m_inputData->leftBorderY, m_inputData->rightBorderY);

            double f1 = evalFunc(x1_new, y1_new);
            double f2 = evalFunc(x2_new, y2_new);

            double better_x, better_y, worse_x, worse_y;
            if ((m_inputData->minOrMax==0 && f2<f1) || (m_inputData->minOrMax==1 && f2>f1)) {
                better_x=x2_new; better_y=y2_new; worse_x=x1_new; worse_y=y1_new;
            } else {
                better_x=x1_new; better_y=y1_new; worse_x=x2_new; worse_y=y2_new;
            }

            double dirX = better_x - worse_x;
            double dirY = better_y - worse_y;
            double norm = std::sqrt(dirX*dirX + dirY*dirY);
            if (norm < 1e-12) break;
            dirX /= norm; dirY /= norm;

            x1 = std::clamp(better_x + h*dirX, m_inputData->leftBorderX, m_inputData->rightBorderX);
            y1 = std::clamp(better_y + h*dirY, m_inputData->leftBorderY, m_inputData->rightBorderY);
            x2 = std::clamp(worse_x + h*dirX, m_inputData->leftBorderX, m_inputData->rightBorderX);
            y2 = std::clamp(worse_y + h*dirY, m_inputData->leftBorderY, m_inputData->rightBorderY);

            if (norm < eps) break;
            iter++;
        }
        double f_val = evalFunc(x1, y1);
        std::cout << "[RESULT] x=" << x1 << ", y=" << y1 << ", f=" << f_val << "\n";

    } catch (...) { return GDResult::Fail; }
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

    constexpr int max_iter = 10000;
    int iter = 0;

    while (iter < max_iter) {
        auto current_time = std::chrono::steady_clock::now();
        if (std::chrono::duration<double>(current_time - start_time).count() >= max_seconds)
            break;

        double numerator = (x2 - x1)*(x2 - x1)*(f2 - f3) - (x2 - x3)*(x2 - x3)*(f2 - f1);
        double denominator = 2.0 * ((x2 - x1)*(f2 - f3) - (x2 - x3)*(f2 - f1));

        double x_min;
        if (std::fabs(denominator) < 1e-12) {
            double step = 0.5 * (x2 - x3);
            if (std::fabs(step) < 1e-8) step = delta * 0.1;
            x_min = x1 - step;
        } else {
            x_min = x2 - numerator / denominator;
        }

        x_min = std::min(std::max(x_min, left_bound), right_bound);
        double f_min = func(x_min);
        if (!std::isfinite(f_min)) f_min = f1 + 1.0;

        // --- Останов по функции и координатам ---
        if (std::fabs(f_min - f1) < tol * 0.5 || std::fabs(x_min - x1) < tol)
            return x_min;

        // Сдвигаем точки
        x3 = x2; f3 = f2;
        x2 = x1; f2 = f1;
        x1 = x_min; f1 = f_min;

        iter++;
    }

    return x1;
}



};

#endif // GRADIENTDESCENT_GRADIENTDESCENT_HPP_
