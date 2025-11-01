//
// Created on 25 Oct, 2025
//  by alecproj
//

#include <iostream>
#include <GradientDescent/GradientDescent.hpp>
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

//Используется для обработки вводимых символов
template<typename T>
T safeReadOrNaN() {
	T value;
	std::cin >> value;
	if (std::cin.fail()) {
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		return std::numeric_limits<T>::quiet_NaN(); // возвращаем NaN
	}
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	return value;
}



GradientInput getInputParameters() {
	GradientInput input;

	std::cout << "Введите функцию f(x,y) (например: x^2 + y^2): ";
	std::getline(std::cin, input.function);
/*
	std::cout << "Введите начальное приближение x0: ";
	input.initialApproximationX_0 = safeReadOrNaN<double>();

	std::cout << "Введите начальное приближение y0: ";
	input.initialApproximationY_0 = safeReadOrNaN<double>();

	std::cout << "Введите коэффициентный шаг h: ";
	input.coefficientStep = safeReadOrNaN<double>();

	std::cout << "Введите точность результата eps: ";
	input.resultAccuracy = safeReadOrNaN<double>();

	std::cout << "Введите точность вычислений (calc_eps): ";
	input.calculationAccuracy = safeReadOrNaN<double>();

	std::cout << "Введите левую границу X: ";
	input.leftBorderX = safeReadOrNaN<double>();

	std::cout << "Введите правую границу X: ";
	input.rightBorderX = safeReadOrNaN<double>();

	std::cout << "Введите левую границу Y: ";
	input.leftBorderY = safeReadOrNaN<double>();

	std::cout << "Введите правую границу Y: ";
	input.rightBorderY = safeReadOrNaN<double>();
*/
	return input;
}




int main()
{
    //testMuparser();
    using AlgoType = GradientDescent<MockReporter>;
    MockReporter reporter{};
    AlgoType algo{&reporter};

	// Получаем данные от пользователя
    const GradientInput data = getInputParameters();


    auto rv = algo.setInputData(&data);
    if (rv == GDResult::Success) {
        rv = algo.solve();
    }

    return 0;
}
