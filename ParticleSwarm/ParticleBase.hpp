//
// Created on 23 Nov, 2025
//  by alecproj
//

#ifndef PARTICLESWARM_PARTICLEBASE_HPP_
#define PARTICLESWARM_PARTICLEBASE_HPP_

#include "Common.hpp"
#include <muParser.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace PS {

class ParticleBase {

    static constexpr double gradient_epsilon{ 1e-16 };

public:

    ParticleBase();

    Result checkFunction(const std::string &function);

    /* Базовые методы алгоритма, не требующие репортера */

protected:
    mu::Parser m_parser;
    double m_x, m_y;
    std::vector<std::string> m_non_diff_functions; // TODO -> static constexpr
    double m_computationPrecision;
    double m_resultPrecision;
    int m_computationDigits;
    int m_resultDigits;

    void resetAlgorithmState();

    void initializeParser(const std::string& function);
    double evaluateFunction(double x, double y);

    double partialDerivativeX(double x, double y);
    double partialDerivativeY(double x, double y);

    Result validateFunctionSyntax(const std::string& function);
    Result checkFunctionDifferentiability(const std::string& function);
};

} // namespace GA

#endif // PARTICLESWARM_PARTICLEBASE_HPP_
