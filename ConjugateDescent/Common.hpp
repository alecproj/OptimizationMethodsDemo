//
// Created on 25 Oct, 2025
//  by alecproj
//

#ifndef CONJUGATEDESCENT_COMMON_HPP_
#define CONJUGATEDESCENT_COMMON_HPP_

struct ConjugateInput {
    long double approximation;
    bool extended;
};

enum class CDResult : int {
    InvalidInput    = -2,
    Fail            = -1,
    Success         = 0
};

#endif // CONJUGATEDESCENT_COMMON_HPP_
