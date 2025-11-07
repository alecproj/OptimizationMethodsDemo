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
        //if (!m_inputData || !m_reporter || m_reporter->begin() == 0) {
        if (!m_inputData) return GDResult::Fail;

        if (!m_inputData->extended) {
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
                    static int tmp;
                    auto current_time = std::chrono::steady_clock::now();
                    double elapsed = std::chrono::duration<double>(current_time - start_time).count();
                    if (elapsed >= max_seconds) break;

                    // Численный градиент (частные производные первый способ)
                    double fx = (evalFunc(x + delta, y) - evalFunc(x - delta, y)) / (2.0 * delta);
                    double fy = (evalFunc(x, y + delta) - evalFunc(x, y - delta)) / (2.0 * delta);

                    /*
                    double fx = 0.0;
                    double fy = 0.0;

                    try {
                        // Вычисляем производные по x и y через muParserX::Diff
                        // первый аргумент — указатель на переменную (та же переменная, что передана в DefineVar)
                        // второй аргумент — текущее значение переменной (x или y)
                        // третий аргумент — шаг (delta)
                        fx = p.Diff(&x, x, delta);
                        fy = p.Diff(&y, y, delta);
                    } catch (mu::Parser::exception_type &e) {
                        std::cerr << "Ошибка при вычислении производных (Diff): " << e.GetMsg() << "\n";
                        return GDResult::Fail;
                    } catch (...) {
                        std::cerr << "Неизвестная ошибка при Diff().\n";
                        return GDResult::Fail;
                    }
                    */

                    double grad_norm = std::sqrt(fx*fx + fy*fy);

                    // Отладочный вывод
                    double f_val = evalFunc(x, y);
                    std::cout << "[DEBUG] n = " << tmp
                              << ", x=" << x
                              << ", y=" << y
                              << ", f(x,y)=" << f_val
                              <<", df/dx=" << fx
                              << ", df/dy=" << fy
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

                    tmp++;
                }

            } catch (mu::Parser::exception_type &e) {
                std::cerr << "Ошибка muParser во время решения: " << e.GetMsg() << "\n";
                return GDResult::Fail;
            } catch (...) {
                std::cerr << "Неизвестная ошибка в GradientDescent::solve\n";
                return GDResult::Fail;
            }
        }

        // ====== Метод наискорейшего спуска ======
// ====== Метод наискорейшего спуска ======
if (m_inputData->extended) {
    try {
        mu::Parser p;
        double x = m_inputData->initialApproximationX_0;
        double y = m_inputData->initialApproximationY_0;
        p.DefineVar("x", &x);
        p.DefineVar("y", &y);
        p.SetExpr(m_inputData->function);

        const double eps = m_inputData->resultAccuracy;
        const double delta = m_inputData->calculationAccuracy;
        const double max_step = 1.0;

        auto evalFunc = [&](double X, double Y) -> double {
            x = X; y = Y;
            return p.Eval();
        };

        auto start_time = std::chrono::steady_clock::now();

        while (true) {
            static int iter = 0;

            constexpr double max_seconds = 60.0;
            auto current_time = std::chrono::steady_clock::now();
            double elapsed = std::chrono::duration<double>(current_time - start_time).count();
            if (elapsed >= max_seconds) break;

            double fx = p.Diff(&x, x, delta);
            double fy = p.Diff(&y, y, delta);
            double grad_norm = std::sqrt(fx*fx + fy*fy);



            if (grad_norm < eps) {

                // ======= ОТЛАДОЧНЫЙ ВЫВОД =======
                double f_val = evalFunc(x, y);
                std::cout << "[DEBUG] iter=" << iter
                          << ", x=" << x
                          << ", y=" << y
                          << ", f(x,y)=" << f_val
                          << ", df/dx=" << fx
                          << ", df/dy=" << fy
                          << ", |grad|=" << grad_norm << "\n";
                // ================================

                std::cout << "[INFO] Модуль градиента < eps, выход из метода наискорейшего спуска.\n";
                break;
            }

            auto phi = [&](double h_val) -> double {
                return evalFunc(x - h_val * fx, y - h_val * fy);
            };

            double h0 = std::max(delta, 1e-6);
            double h_opt = powell_method(phi, h0, delta, m_inputData->calculationAccuracy, 0.0, max_step, max_seconds);

            x = x - h_opt * fx;
            y = y - h_opt * fy;

            x = std::min(std::max(x, m_inputData->leftBorderX), m_inputData->rightBorderX);
            y = std::min(std::max(y, m_inputData->leftBorderY), m_inputData->rightBorderY);

            iter++;
        }

    } catch (mu::Parser::exception_type &e) {
        std::cerr << "Ошибка muParser в методе наискорейшего спуска: " << e.GetMsg() << "\n";
    } catch (...) {
        std::cerr << "Неизвестная ошибка в методе наискорейшего спуска.\n";
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
                std::cerr << "Недопустимый идентификатор в функции: " << token << std::endl;
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

    double x1 = x0;
    double x2 = x1 + delta;
    double x3 = x1 - delta;

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

        if (std::fabs(denominator) < 1e-12) {
            double step = (x2 - x3) * 0.5;
            if (step == 0.0) step = delta;
            double x_min = x1 - step;
            x_min = std::min(std::max(x_min, left_bound), right_bound);
            return x_min;
        }

        double x_min = x2 - numerator / denominator;
        x_min = std::min(std::max(x_min, left_bound), right_bound);

        double f_min = func(x_min);
        if (!std::isfinite(f_min)) f_min = f1 + 1.0;

        if (std::fabs(x_min - x1) < tol) return x_min;

        x3 = x2; f3 = f2;
        x2 = x1; f2 = f1;
        x1 = x_min; f1 = f_min;
    }

    return x1;
}


};

#endif // GRADIENTDESCENT_GRADIENTDESCENT_HPP_
