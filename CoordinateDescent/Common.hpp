//
// Created on 25 Oct, 2025
//  by alecproj
//

#ifndef COORDINATEDESCENT_COMMON_HPP_
#define COORDINATEDESCENT_COMMON_HPP_

struct CoordinateInput {
    long double approximation;
    bool extended;
};

enum class CDResult : int {
    InvalidInput    = -2,
    Fail            = -1,
    Success         = 0
};

#endif // COORDINATEDESCENT_COMMON_HPP_
