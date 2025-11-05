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
    static constexpr long double MIN_STEP{1e-10};       // Минимальный шаг (до 10^-10)
    static constexpr long double STEP_REDUCTION{0.5};   // Коэффициент снижения шага
    static constexpr long double MAX_STEP{1.0};         // Максимальный шаг (до 1.0)

public:

    CoordinateDescent(Reporter *reporter) :
        m_inputData{nullptr},
        m_reporter{reporter}
        m_parser{}; // Инициализация парсера muParser (проверить)
    {
    }

    CDResult setInputData(const CoordinateInput *data)
    {
        if (!data) {
            return CDResult::InvalidInput;
        }

        // ВАЛИДАЦИЯ ВХОДНЫХ ДАННЫХ

        // Проверка функции
        if (data->function.empty()) {
            return CDResult::InvalidInput;
        }

        // Проверка типа алгоритма
        if (data->algorithm_type != AlgorithmType::BASIC_COORDINATE_DESCENT &&
            data->algorithm_type != AlgorithmType::STEEPEST_COORDINATE_DESCENT) {
            return CDResult::InvalidInput;
        }

        // Проверка типа экстремума
        if (data->extremum_type != ExtremumType::MINIMUM &&
            data->extremum_type != ExtremumType::MAXIMUM) {
            return CDResult::InvalidInput;
        }

        // Проверка типа шага
        if (data->step_type != StepType::CONSTANT &&
            data->step_type != StepType::COEFFICIENT &&
            data->step_type != StepType::ADAPTIVE) {
            return CDResult::InvalidInput;
        }

        // Проверка корректности границ X (вопрос про равенство)
        if (data->x_left_bound >= data->x_right_bound) {
            return CDResult::InvalidInput;
        }

        // Проверка корректности границ Y (вопрос про равенство)
        if (data->y_left_bound >= data->y_right_bound) {
            return CDResult::InvalidInput;
        }

        // Проверка начального приближения X
        if (data->initial_x < data->x_left_bound || data->initial_x > data->x_right_bound) {
            return CDResult::OutOfBounds;
        }

        // Проверка начального приближения Y
        if (data->initial_y < data->y_left_bound || data->initial_y > data->y_right_bound) {
            return CDResult::OutOfBounds;
        }

        // Проверка точности результата
        if (data->result_precision <= 0.0 || data->result_precision > 1.0) {
            return CDResult::InvalidInput;
        }

        // Проверка точности вычислений
        if (data->computation_precision <= 0.0 || data->computation_precision > 1.0) {
            return CDResult::InvalidInput;
        }

     /* // Проверка соотношения точностей (точность вычислений должно быть в 10 раз точнее результата)
        static constexpr long double MIN_PRECISION_RATIO = 10.0;
        if (data->computation_precision * MIN_PRECISION_RATIO > data->result_precision) {
            return CDResult::InvalidInput;
        }*/

        // Проверка что точность вычислений меньше точности результата
        if (data->computation_precision >= data->result_precision) {
            return CDResult::InvalidInput;
        }

        // Дальше проверки шага, коэффа, границы, шаги, синтаксис функции


        // Сначала проверить все поля на корректность
        m_inputData = data;
        return CDResult::Success;
    }

    CDResult solve()
    {
        if (!m_inputData || !m_reporter || m_reporter->begin() == 0) {
            return CDResult::Fail;
        }
       
        if (m_inputData->extended) { // тут исправить 
            // Расширенный функционал // тут исправить 
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

};

#endif // COORDINATEDESCENT_COORDINATEDESCENT_HPP_ 
