//
// Created on 25 Oct, 2025
//  by alecproj
//

#ifndef COORDINATEDESCENT_COORDINATEDESCENT_HPP_
#define COORDINATEDESCENT_COORDINATEDESCENT_HPP_

#include <cmath>
#include <functional>
#include <stdexcept> //golden_section()
#include <CoordinateDescent/Common.hpp>

template <typename Reporter>
class CoordinateDescent {

    // Пример объявления и инициализации констант
    static constexpr auto CONST_VALUE_EXAMLE{0};

public:

    CoordinateDescent(Reporter *reporter) :
        m_inputData{nullptr},
        m_reporter{reporter}
    {
    }

    CDResult setInputData(const CoordinateInput *data)
    {
        if (!data) {
            return CDResult::InvalidInput;
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

        if (m_inputData->extended) {
            // Расширенный функционал
        }

        // Процесс решения и записи отчета (разбить на приватные функции)

        if (m_reporter->end() == 0) {
            return CDResult::Success;
        }
        return CDResult::Fail;
    }

private:

    const CoordinateInput *m_inputData;
    Reporter *m_reporter;


    // Метод золотого сечения для одномерной минимизации
    static double golden_section(const std::function<double(double)> &func, double left_bound, double right_bound, const double tol = 1e-5) {

        if (!func) {
            throw std::invalid_argument("golden_section: Function pointer is null");
        }
        if (left_bound > right_bound) {
            std::swap(left_bound, right_bound);
        }
        if (tol <= 0) {
            throw std::invalid_argument("Tolerance must be positive");
        }
        if (tol >= std::abs(right_bound - left_bound)) {
            throw std::invalid_argument("Tolerance is too large for the given interval");
        }


        static const double PHI = (1 + std::sqrt(5)) / 2;

        // Вычисляем внутренние точки интервала
        double left_internal = right_bound - (right_bound - left_bound) / PHI;
        double right_internal = left_bound + (right_bound - left_bound) / PHI;

        // Вычисляем значения функции в этих точках
        double f_left = func(left_internal);
        double f_right = func(right_internal);

        // Пока длина интервала больше заданной точности
        while (std::abs(right_bound - left_bound) > tol) {

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

};

#endif // COORDINATEDESCENT_COORDINATEDESCENT_HPP_ 
