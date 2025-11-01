//
// Created on 25 Oct, 2025
//  by alecproj
//

#ifndef COORDINATEDESCENT_COORDINATEDESCENT_HPP_
#define COORDINATEDESCENT_COORDINATEDESCENT_HPP_

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
