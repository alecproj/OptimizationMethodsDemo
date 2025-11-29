//
// Created on 23 Nov, 2025
//  by alecproj
//

#include "GeneticBase.hpp"
#include <glog/logging.h>

namespace GA {

GeneticBase::GeneticBase() : m_rnd(std::random_device{}())
{
    resetAlgorithmState();
}

void GeneticBase::resetAlgorithmState()
{
    m_x = 0.0;
    m_y = 0.0;
    m_inputData = nullptr;
    m_config = GAConfig();
    m_population.clear();
    m_currentGeneration = 0;
    m_functionCalls = 0;
    
    try {
        m_parser.SetExpr("0");
        m_parser.DefineVar("x", &m_x);
        m_parser.DefineVar("y", &m_y);
    }catch (...) {

    }
}

// Инициализация 
Result GeneticBase::initialize(const InputData* inputData, const GAConfig* config)
{
    if (!inputData) {
        return Result::InvalidInput;
    }

    m_inputData = inputData;
    m_config = config ? *config : GAConfig();

    // Инициализация парсера
    try {
        initializeParser(inputData->function);
    }catch (const mu::Parser::exception_type& e) {
        LOG(ERROR) << "Ошибка Парсера: " << e.GetMsg();
        return Result::ParseError;
    }

    // Инициализация кодировщика 
    EncodingConfig encoderConfig(
        m_config.bits_per_variable,
        inputData->x_left_bound, inputData->x_right_bound,
        inputData->y_left_bound, inputData->y_right_bound
    );
    try {
        m_encoder = GeneticEncoder(encoderConfig);
    } catch (const std::exception& e) {
        LOG(ERROR) << "Ошибка создания кодировщика: " << e.what();
        return Result::InvalidInput;
    }
    
    // Инициализация популяции
    //initializePopulation();
    LOG(INFO) << "Генетический алгоритм начал работу с количеством популяций: "
              << m_config.population_size;

    return Result::Success;
}

void GeneticBase::clear()
{
    resetAlgorithmState();
}

// Проверка на корректность валидации функции 
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


void GeneticBase::initializeParser(const std::string& function)
{
    m_parser.SetExpr(function);
    m_parser.DefineVar("x", &m_x);
    m_parser.DefineVar("y", &m_y);
    m_functionCalls = 0;
    m_currentGeneration = 0;
}

double GeneticBase::evaluateFitness(double x, double y)
{
    try {
        m_x = x;
        m_y = y;
        double functionValue = m_parser.Eval();
        m_functionCalls++;
        return functionValue;

    }catch (const std::exception& e) {
        LOG(ERROR) << "Ошибка вычисления функции в точке (" << x << ", " << y << "): " << e.what();
        return worstPossibleFitness();
    }
}

// Возвращение худших значений при ошибке
double GeneticBase::worstPossibleFitness() const {
    if (m_inputData->extremum_type == ExtremumType::MAXIMUM) {
        // Для максимума - худшее значение это -∞
        return -std::numeric_limits<double>::max();
    }
    else {
        // Для минимума - худшее значение это +∞
        return std::numeric_limits<double>::max();
    }
}

// Cортировка популяции в зависимости от вида экстремума
void GeneticBase::sortPopulation()
{
    if (m_inputData->extremum_type == ExtremumType::MAXIMUM) {
        // Для максимума - сортируем по УБЫВАНИЮ fitness
        // Чем БОЛЬШЕ значение функции, тем ЛУЧШЕ особь
        std::sort(m_population.begin(), m_population.end(),
            [](const Individual& a, const Individual& b) {
                return a.fitness > b.fitness;
            });
    }else {
        // Для минимума - сортируем по ВОЗРАСТАНИЮ fitness
        // Чем МЕНЬШЕ значение функции, тем ЛУЧШЕ особь
        std::sort(m_population.begin(), m_population.end(),
            [](const Individual& a, const Individual& b) {
                return a.fitness < b.fitness;
            });
    }
    // После сортировка первая особь [0] - всегда лучшая
}

//Возвращение лучшего значения функции
double GeneticBase::getBestFitness() const
{
    if (m_population.empty()) return 0.0;
    // Возвращаем реальное значение функции лучшей особи
    return m_population[0].fitness;
}

} // namespace GA
