//
// Created on 25 Oct, 2025
//  by alecproj
//

#ifndef CONJUGATEGRADIENT_COORDINATEGRADIENT_HPP_
#define CONJUGATEGRADIENT_COORDINATEGRADIENT_HPP_

#include <CoordinateDescent/Common.hpp>
#include <muParser.h>
#include <vector>
#include <cmath>
#include <algorithm>

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
        }
        double getX() const { return m_x; }                             // Получить X
        double getY() const { return m_y; }                             // Получить Y
        int getIterations() const { return m_iterations; }              // Получить кол-во итераций
        int getFunctionCalls() const { return m_function_calls; }       // Получить кол-во вызовов функции
        double getOptimumValue() { return evaluateFunction(m_x, m_y); } // Вычисление значение функции в финальной точке

        CDResult setInputData(const ConjugateInput* data)
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

        const ConjugateInput* m_inputData;
        Reporter* m_reporter;

    };

}

#endif // CONJUGATEGRADIENT_CONJUGATEGRADIENT_HPP_ 
