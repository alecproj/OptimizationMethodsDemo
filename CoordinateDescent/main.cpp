//
// Created on 25 Oct, 2025
//  by alecproj
//

#include <CoordinateDescent/CoordinateDescent.hpp>
#include <muParser.h>
#include <iostream>

// Класс-заглушка - реализовывать не нужно!
class MockReporter {
public:
    int begin(){ return 0; }
    int end() { return 0; }
};

double MySqr(double a_fVal) {  return a_fVal*a_fVal; }

void testMuparser()
{
    try
	{
		double var_a = 1;
		mu::Parser p;
		p.DefineVar("a", &var_a); 
		p.DefineFun("MySqr", MySqr); 
		p.SetExpr("MySqr(a)*_pi+min(10,a)");

		for (std::size_t a=0; a<5; ++a)
		{
			var_a = a;  // Change value of variable a
			std::cout << p.Eval() << std::endl;
		}
	}
	catch (mu::Parser::exception_type &e)
	{
		std::cout << e.GetMsg() << std::endl;
	}
}

int main()
{
    testMuparser();

    using AlgoType = CoordinateDescent<MockReporter>;

    MockReporter reporter{};
    AlgoType algo{&reporter};
    CoordinateInput data{};

    auto rv = algo.setInputData(&data);
    if (rv == CDResult::Success) {
        rv = algo.solve();
    }

    return 0;
}
