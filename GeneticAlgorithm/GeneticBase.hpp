//
// Created on 23 Nov, 2025
//  by alecproj
//

#ifndef GENETICALGORITHM_GENETICBASE_HPP_
#define GENETICALGORITHM_GENETICBASE_HPP_

#include "Common.hpp"
#include <muParser.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace GA {

class GeneticBase {

    static constexpr double gradient_epsilon{ 1e-16 };

public:

    GeneticBase();

    Result checkFunction(const std::string &function);

    void selection(/* ... */);
    void crossover(/* ... */);
    /* И другие методы, не требующие репортера */

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

#endif // GENETICALGORITHM_GENETICBASE_HPP_
