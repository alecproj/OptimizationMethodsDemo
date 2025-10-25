//
// Created on 25 Oct, 2025
//  by alecproj
//

#ifndef GRADIENTDESCENT_GRADIENTDESCENT_HPP_
#define GRADIENTDESCENT_GRADIENTDESCENT_HPP_

#include <GradientDescent/GradientInput.hpp>
#include <Application/Reporter.hpp>
#include <Application/Common.hpp>

class GradientDescent {

    // Пример объявления и инициализации констант
    static constexpr auto CONST_VALUE_EXAMLE{0};

public:

    GradientDescent(Reporter *reporter);

    AlgResult setInputData(GradientInput *data);
    AlgResult solve(); 

private:

    GradientInput *m_data;
    Reporter *m_reporter;

};

#endif // GRADIENTDESCENT_GRADIENTDESCENT_HPP_
