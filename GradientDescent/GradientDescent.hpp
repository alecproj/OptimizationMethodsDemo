//
// Created on 25 Oct, 2025
//  by alecproj
//

#ifndef GRADIENTDESCENT_GRADIENTDESCENT_HPP_
#define GRADIENTDESCENT_GRADIENTDESCENT_HPP_

#include <GradientDescent/Common.hpp>
#include <chrono>
#include <cmath>
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

        try {
            mu::Parser p;
            double x = m_inputData->initialApproximationX_0;
            double y = m_inputData->initialApproximationY_0;
            p.DefineVar("x", &x);
            p.DefineVar("y", &y);
            p.SetExpr(m_inputData->function);

            const double h = m_inputData->coefficientStep;
            const double eps = m_inputData->resultAccuracy;
            const double delta = m_inputData->calculationAccuracy;

            auto evalFunc = [&](double X, double Y) -> double {
                x = X; y = Y;
                return p.Eval();
            };

            auto start_time = std::chrono::steady_clock::now();
            constexpr double max_seconds = 60.0;

            while (true) {
                auto current_time = std::chrono::steady_clock::now();
                double elapsed = std::chrono::duration<double>(current_time - start_time).count();
                if (elapsed >= max_seconds) break;

                // Численный градиент
                double fx = (evalFunc(x + delta, y) - evalFunc(x - delta, y)) / (2.0 * delta);
                double fy = (evalFunc(x, y + delta) - evalFunc(x, y - delta)) / (2.0 * delta);

                double grad_norm = std::sqrt(fx*fx + fy*fy);

                // Отладочный вывод
                double f_val = evalFunc(x, y);
                std::cout << "[DEBUG] x=" << x
                          << ", y=" << y
                          << ", f(x,y)=" << f_val
                          << ", |grad|=" << grad_norm << "\n";

                // Условие окончания по норме градиента
                if (grad_norm < eps) {
                    std::cout << "[DEBUG] Модуль градиента < eps, выход из цикла.\n";
                    break;
                }

                // Обновление координат
                x -= h * fx;
                y -= h * fy;

                // Ограничение по границам
                x = std::min(std::max(x, m_inputData->leftBorderX), m_inputData->rightBorderX);
                y = std::min(std::max(y, m_inputData->leftBorderY), m_inputData->rightBorderY);
            }

        } catch (mu::Parser::exception_type &e) {
            std::cerr << "Ошибка muParser во время решения: " << e.GetMsg() << "\n";
            return GDResult::Fail;
        } catch (...) {
            std::cerr << "Неизвестная ошибка в GradientDescent::solve\n";
            return GDResult::Fail;
        }

        if (m_reporter->end() == 0) return GDResult::Success;
        return GDResult::Fail;
    }


private:

    const GradientInput *m_inputData;
    Reporter *m_reporter;
    // Метод золотого сечения для одномерной минимизации
    static double golden_section(const std::function<double(double)> &func, double left_bound, double right_bound, const double tol = 1e-5, double max_seconds = 60.0) {

        if (!func) {
            throw std::invalid_argument("golden_section: Function pointer is null");
        }
        if (left_bound > right_bound) {
            std::swap(left_bound, right_bound);
        }
        if (tol <= 0) {
            throw std::invalid_argument("golden_section: Tolerance must be positive");
        }
        if (tol >= std::fabs(right_bound - left_bound)) {
            throw std::invalid_argument("golden_section: Tolerance is too large for the given interval");
        }
        if (max_seconds <= 0) {
            throw std::invalid_argument("golden_section: Max_seconds must be positive");
        }

        static const double PHI = (1 + std::sqrt(5)) / 2;

        // Вычисляем внутренние точки интервала
        double left_internal = right_bound - (right_bound - left_bound) / PHI;
        double right_internal = left_bound + (right_bound - left_bound) / PHI;

        // Вычисляем значения функции в этих точках
        double f_left = func(left_internal);
        double f_right = func(right_internal);


        const auto start_time = std::chrono::steady_clock::now();

        // Пока длина интервала больше заданной точности
        while (std::fabs(right_bound - left_bound) > tol) {
            auto current_time = std::chrono::steady_clock::now();
            if (std::chrono::duration<double> elapsed = current_time - start_time; elapsed.count() >= max_seconds) {
                throw std::runtime_error("golden_section: Time limit exceeded before convergence");
            }
            if (f_left < f_right) {
                right_bound = right_internal;
                right_internal = left_internal;
                f_right = f_left;
                left_internal = right_bound - (right_bound - left_bound) / PHI;
                f_left = func(left_internal);
            } else {
                left_bound = left_internal;
                left_internal = right_internal;
                f_left = f_right;
                right_internal = left_bound + (right_bound - left_bound) / PHI;
                f_right = func(right_internal);
            }
        }
        return (f_left < f_right) ? left_internal : right_internal;
    }

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
                std::cerr << "Недопустимый идентификатор в функции: " << token << std::endl;
                return false;
            }

            s = match.suffix().str();
        }

        return true;
    }

};

#endif // GRADIENTDESCENT_GRADIENTDESCENT_HPP_
