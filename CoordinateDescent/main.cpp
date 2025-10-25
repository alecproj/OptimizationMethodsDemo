//
// Created on 25 Oct, 2025
//  by alecproj
//

#include <CoordinateDescent/CoordinateDescent.hpp>

// Класс-заглушка - реализовывать не нужно!
class MockReporter {
public:
    int begin(){ return 0; }
    int end() { return 0; }
};

int main()
{
    using AlgoType = typename CoordinateDescent<MockReporter>;

    MockReporter reporter{};
    AlgoType algo{&reporter};
    CoordinateInput data{};

    auto rv = algo.setInputData(&data);
    if (rv == 0) {
        rv = algo.solve();
    }

    return 0;
}
