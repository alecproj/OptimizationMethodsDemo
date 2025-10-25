//
// Created on 25 Oct, 2025
//  by alecproj
//

#include <GradientDescent/GradientDescent.hpp>

// Класс-заглушка - реализовывать не нужно!
class MockReporter {
public:
    int begin(){ return 0; }
    int end() { return 0; }
};

int main()
{
    using AlgoType = typename GradientDescent<MockReporter>;

    MockReporter reporter{};
    AlgoType algo{&reporter};
    GradientInput data{};

    auto rv = algo.setInputData(&data);
    if (rv == 0) {
        rv = algo.solve();
    }

    return 0;
}
