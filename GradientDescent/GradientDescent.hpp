//
// Created on 25 Oct, 2025
//  by alecproj
//

#ifndef GRADIENTDESCENT_GRADIENTDESCENT_HPP_
#define GRADIENTDESCENT_GRADIENTDESCENT_HPP_

#include <GradientDescent/Common.hpp>

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

};

#endif // GRADIENTDESCENT_GRADIENTDESCENT_HPP_
