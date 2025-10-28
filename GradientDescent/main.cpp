//
// Created on 25 Oct, 2025
//  by alecproj
//

#include <iostream>
#include <GradientDescent/GradientDescent.hpp>
#include "LatexParser.hpp"

// Класс-заглушка - реализовывать не нужно!
class MockReporter {
public:
    int begin(){ return 0; }
    int end() { return 0; }
};

int main()
{
    using AlgoType = GradientDescent<MockReporter>;

    MockReporter reporter{};
    AlgoType algo{&reporter};
    GradientInput data{};



    auto rv = algo.setInputData(&data);
    if (rv == GDResult::Success) {
        rv = algo.solve();
    }

    return 0;
}
