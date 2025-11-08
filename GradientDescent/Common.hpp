//
// Created on 25 Oct, 2025
//  by alecproj
//

#ifndef GRADIENTDESCENT_COMMON_HPP_
#define GRADIENTDESCENT_COMMON_HPP_

// Структура для хранения всех входных параметров
struct GradientInput {
    std::string function; // Строка с функцией
    double initialApproximationX_0;            // начальное приближение X
    double initialApproximationY_0;            // начальное приближение Y
    double coefficientStep;             // коэффициентный шаг
    double resultAccuracy;           // точность результата
    double calculationAccuracy;      // точность вычислений
    double leftBorderX;         // левая граница X
    double rightBorderX;         // правая граница X
    double leftBorderY;         // левая граница Y
    double rightBorderY;         // правая граница Y
    int extended = 0;
};

enum class GDResult : int {
    InvalidInput    = -2,
    Fail            = -1,
    Success         = 0
};

#endif // GRADIENTDESCENT_COMMON_HPP_
