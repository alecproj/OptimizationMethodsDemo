//
// Created on 23 Nov, 2025
//  by alecproj
//

#include "GeneticBase.hpp"
#include <glog/logging.h>

namespace GA {

GeneticBase::GeneticBase()
{
}

void GeneticBase::resetAlgorithmState()
{
    m_x = 0.0;
    m_y = 0.0;
    // Инициализация списка недифференцируемых функций
    // TODO Сделать static constexpr
    m_non_diff_functions = {
        "abs(", "|", "sign(", "floor(", "ceil(", "round(",
        "fmod(", "mod(", "rand(", "max(", "min(", "random"
    };
    // TODO
}

void GeneticBase::initializeParser(const std::string& function)
{
    m_parser.SetExpr(function);
    m_parser.DefineVar("x", &m_x);
    m_parser.DefineVar("y", &m_y);
}

double GeneticBase::evaluateFunction(double x, double y)
{
    m_x = x;
    m_y = y;
    try {
        return m_parser.Eval();
    }
    catch (...) {
        throw std::runtime_error("Ошибка вычисления функции в точке");
    }
}

double GeneticBase::partialDerivativeX(double x, double y)
{
    double x_old = m_x, y_old = m_y;
    m_y = y; // Фиксируем y
    double derivative = m_parser.Diff(&m_x, x, m_computationPrecision);
    m_x = x_old;
    m_y = y_old;
    return derivative;
}

double GeneticBase::partialDerivativeY(double x, double y)
{
    double x_old = m_x, y_old = m_y;
    m_x = x; // Фиксируем x
    double derivative = m_parser.Diff(&m_y, y, m_computationPrecision);
    m_x = x_old;
    m_y = y_old;
    return derivative;
}

Result GeneticBase::validateFunctionSyntax(const std::string& function)
{
    try {
        mu::Parser test_parser;
        double test_x = 0.0;
        double test_y = 0.0;
        test_parser.SetExpr(function);
        test_parser.DefineVar("x", &test_x);
        test_parser.DefineVar("y", &test_y);

        // Пробуем вычислить в тестовой точке
        test_parser.Eval();

        return Result::Success;
    }
    catch (const mu::Parser::exception_type& e) {
        return Result::ParseError;
    }
    catch (...) {
        return Result::ParseError;
    }
}

Result GeneticBase::checkFunctionDifferentiability(const std::string& function)
{
    try {

        // Предварительная проверка на известные недифференцируемые функции
        std::vector<std::string> non_diff_functions = {
            "abs(", "|", "sign(", "floor(", "ceil(", "round(",
            "fmod(", "mod(", "rand(", "max(", "min(", "random"
        };

        std::string func_lower = function;
        std::transform(func_lower.begin(), func_lower.end(), func_lower.begin(), ::tolower);

        for (const auto& non_diff_func : m_non_diff_functions) {
            std::string non_diff_lower = non_diff_func;
            std::transform(non_diff_lower.begin(), non_diff_lower.end(), non_diff_lower.begin(), ::tolower);

            if (func_lower.find(non_diff_lower) != std::string::npos) {
                LOG(ERROR) << "Обнаружена потенциально недифференцируемая функция: " << non_diff_func;
                // TODO Перенести в GeneticAlgorithm
                // m_reporter->insertMessage("Обнаружена потенциально недифференцируемая функция");
                return Result::NonDifferentiableFunction;
            }
        }

        // Проверка численной дифференцируемости
        mu::Parser test_parser;
        double test_x = 0.0;
        double test_y = 0.0;
        test_parser.SetExpr(function);
        test_parser.DefineVar("x", &test_x);
        test_parser.DefineVar("y", &test_y);

        const int TEST_POINTS = 8;
        std::vector<std::pair<double, double>> test_points;

        // TODO Учесть изменения
        double center_x = /* m_inputData ? m_inputData->initial_x :*/ 0.0;
        double center_y = /*m_inputData ? m_inputData->initial_y :*/ 0.0;

        for (int i = 0; i < TEST_POINTS; i++) {
            double angle = 2.0 * M_PI * i / TEST_POINTS;
            double radius = 0.1;
            test_points.push_back({
                center_x + radius * std::cos(angle),
                center_y + radius * std::sin(angle)
                });
        }

        for (const auto& point : test_points) {
            test_x = point.first;
            test_y = point.second;

            try {
                double func_value = test_parser.Eval();
                // Проверяем производные с разной точностью
                double deriv_x1 = 0.0, deriv_x2 = 0.0;
                double deriv_y1 = 0.0, deriv_y2 = 0.0;

                // Первая попытка с обычной точностью
                deriv_x1 = test_parser.Diff(&test_x, test_x, 1e-6);
                deriv_y1 = test_parser.Diff(&test_y, test_y, 1e-6);

                // Вторая попытка с другой точностью для проверки стабильности
                deriv_x2 = test_parser.Diff(&test_x, test_x, 1e-7);
                deriv_y2 = test_parser.Diff(&test_y, test_y, 1e-7);

                // Проверяем, что производные не "взрываются"
                if (std::isnan(deriv_x1) || std::isinf(deriv_x1) ||
                    std::isnan(deriv_y1) || std::isinf(deriv_y1) ||
                    std::isnan(deriv_x2) || std::isinf(deriv_x2) ||
                    std::isnan(deriv_y2) || std::isinf(deriv_y2)) {
                    // TODO Перенести в GeneticAlgorithm
                    // LOG(ERROR) << "Производная не определена в точке ("
                    //     << test_x << ", " << test_y << ")";
                    // m_reporter->insertMessage("Производная не определена в точке ("
                    //     + std::to_string(test_x) + ", " + std::to_string(test_y) + ")");
                    return Result::NonDifferentiableFunction;
                }
            }
            catch (const mu::Parser::exception_type& e) {
                LOG(ERROR) << "Функция не дифференцируема в точке ("
                    << test_x << ", " << test_y << "): " << e.GetMsg();
                // TODO Перенести в GeneticAlgorithm
                // m_reporter->insertMessage("Функция не дифференцируема в точке ("
                //     + std::to_string(test_x) + ", " + std::to_string(test_y) + ")");
                return Result::NonDifferentiableFunction;
            }
            catch (const std::exception& e) {
                LOG(ERROR) << "Ошибка дифференцирования в точке ("
                    << test_x << ", " << test_y << "): " << e.what();
                // TODO Перенести в GeneticAlgorithm
                // m_reporter->insertMessage("Ошибка дифференцирования в точке ("
                //     + std::to_string(test_x) + ", " + std::to_string(test_y) + ")");

                return Result::NonDifferentiableFunction;
            }
        }
        // TODO Перенести в GeneticAlgorithm
        // LOG(INFO) << "Функция прошла проверку дифференцируемости";
        // m_reporter->insertMessage("Функция прошла проверку дифференцируемости");
        return Result::Success;

    }
    catch (const mu::Parser::exception_type& e) {
        LOG(ERROR) << "Ошибка парсера при проверке дифференцируемости: " << e.GetMsg();
        // TODO Перенести в GeneticAlgorithm
        // m_reporter->insertMessage("Ошибка парсера при проверке дифференцируемости: ");
        return Result::ParseError;
    }
    catch (const std::exception& e) {
        LOG(ERROR) << "Ошибка при проверке дифференцируемости: " << e.what();
        // TODO Перенести в GeneticAlgorithm
        // m_reporter->insertMessage("Общая ошибка при проверке дифференцируемости: ");
        return Result::ComputeError;
    }
}

} // namespace GA
