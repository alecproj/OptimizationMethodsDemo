//
// Created on 25 Oct, 2025
//  by alecproj
//

#ifndef GRADIENTDESCENT_COMMON_HPP_
#define GRADIENTDESCENT_COMMON_HPP_

struct GradientInput {
    long double approximation;
};

enum class GDResult : int {
    InvalidInput    = -2,
    Fail            = -1,
    Success         = 0
};

#endif // GRADIENTDESCENT_COMMON_HPP_
