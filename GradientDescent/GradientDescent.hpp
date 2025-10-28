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
#include <stdexcept>
#include "LatexParser.hpp"

template <typename Reporter>
class GradientDescent {

    // Пример объявления и инициализации констант
    static constexpr auto CONST_VALUE_EXAMLE{0};

public:

    GradientDescent(Reporter *reporter) :
        m_inputData{nullptr},
        m_reporter{reporter}
    {
    }

    GDResult setInputData(const GradientInput *data)
    {
        if (!data) {
            return GDResult::InvalidInput;
        }
        // Сначала проверить все поля на корректность
        m_inputData = data;
        return GDResult::Success;
    }

    GDResult solve()
    {
        if (!m_inputData || !m_reporter || m_reporter->begin() == 0) {
            return GDResult::Fail;
        }
        
        if (m_inputData->extended) {
            // Расширенный функционал
        }

        // Процесс решения и записи отчета (разбить на приватные функции)

        if (m_reporter->end() == 0) {
            return GDResult::Success;
        }
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

    /*
    Пример передачи аргументов:
    auto f1 = [](double x){ return (x - 2)*(x - 2); };
    double x1 = golden_section(f1, -5, 5, 1e-6);
     */

};

#endif // GRADIENTDESCENT_GRADIENTDESCENT_HPP_
