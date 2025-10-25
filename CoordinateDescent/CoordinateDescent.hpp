//
// Created on 25 Oct, 2025
//  by alecproj
//

#ifndef COORDINATEDESCENT_COORDINATEDESCENT_HPP_
#define COORDINATEDESCENT_COORDINATEDESCENT_HPP_

#include <CoordinateDescent/CoordinateInput.hpp>
#include <Application/Reporter.hpp>
#include <Application/Common.hpp>

class CoordinateDescent {

    // Пример объявления и инициализации констант
    static constexpr auto CONST_VALUE_EXAMLE{0};

public:

    CoordinateDescent(Reporter *reporter);

    AlgResult setInputData(CoordinateInput *data);
    AlgResult solve(); 

private:

    CoordinateInput *m_data;
    Reporter *m_reporter;

};

#endif // COORDINATEDESCENT_COORDINATEDESCENT_HPP_ 
