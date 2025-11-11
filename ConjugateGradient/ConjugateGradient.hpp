//
// Created on 25 Oct, 2025
//  by alecproj
//

#ifndef CONJUGATEGRADIENT_COORDINATEGRADIENT_HPP_
#define CONJUGATEGRADIENT_COORDINATEGRADIENT_HPP_

#include <ConjugateGradient/Common.hpp>

template <typename Reporter>
class ConjugateGradient {

    // Пример объявления и инициализации констант
    static constexpr auto CONST_VALUE_EXAMLE{0};

public:

    ConjugateGradient(Reporter *reporter) :
        m_inputData{nullptr},
        m_reporter{reporter}
    {
    }

    CDResult setInputData(const ConjugateInput *data)
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

    const ConjugateInput *m_inputData;
    Reporter *m_reporter;

};

#endif // CONJUGATEGRADIENT_CONJUGATEGRADIENT_HPP_ 
