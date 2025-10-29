//
// Created on 25 Oct, 2025
//  by alecproj
//

#ifndef COORDINATEDESCENT_COORDINATEDESCENT_HPP_
#define COORDINATEDESCENT_COORDINATEDESCENT_HPP_

#include <CoordinateDescent/Common.hpp>
#include <string>
// #include <exprtk/exprtk.hpp>
#include "exprtk.hpp"

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

    template <typename T>
    void trig_function()
    {
       typedef exprtk::symbol_table<T> symbol_table_t;
       typedef exprtk::expression<T>   expression_t;
       typedef exprtk::parser<T>       parser_t;

       const std::string expression_string =
          "clamp(-1.0, sin(2 * pi * x) + cos(x / 2 * pi), +1.0)";

       T x;

       symbol_table_t symbol_table;
       symbol_table.add_variable("x",x);
       symbol_table.add_constants();

       expression_t expression;
       expression.register_symbol_table(symbol_table);

       parser_t parser;
       parser.compile(expression_string,expression);

       for (x = T(-5); x <= T(+5); x += T(0.001))
       {
          const T y = expression.value();
          printf("%19.15f\t%19.15f\n", x, y);
       }
    }

    CDResult solve()
    {
        if (!m_inputData || !m_reporter || !m_reporter->begin() == 0) {
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

};

#endif // COORDINATEDESCENT_COORDINATEDESCENT_HPP_ 
