//
// Created on 25 Oct, 2025
//  by alecproj
//

#ifndef CONJUGATEGRADIENT_COMMON_HPP_
#define CONJUGATEGRADIENT_COMMON_HPP_

struct ConjugateInput {
    long double approximation;
    bool extended;
};

enum class CDResult : int {
    InvalidInput    = -2,
    Fail            = -1,
    Success         = 0
};

#endif // CONJUGATEGRADIENT_COMMON_HPP_
