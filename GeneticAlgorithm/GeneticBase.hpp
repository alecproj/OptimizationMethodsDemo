//
// Created on 23 Nov, 2025
//  by alecproj
//

#ifndef GENETICALGORITHM_GENETICBASE_HPP_
#define GENETICALGORITHM_GENETICBASE_HPP_

#include "Common.hpp"
#include "GeneticEncoder.hpp"
#include "vector"
#include <random>
#include <muParser.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace GA {

class GeneticBase {


public:

    GeneticBase();
    // Метод инициализации
    Result initialize(const InputData* inputData, const GAConfig* config = nullptr);
    
    // Метод очистки
    void clear();
    // Создание популяции
    void initializePopulation();

    Result runGeneration();
    Result checkFunction(const std::string &function);

    // Геттеры
    double getBestX()         const { return m_population.empty() ? 0.0 : m_population[0].x; }
    double getBestY()         const { return m_population.empty() ? 0.0 : m_population[0].y; }
    double getBestFitness()   const { return m_population.empty() ? 0.0 : m_population[0].fitness; }
    size_t getIteraionts()    const { return m_currentGeneration; }
    size_t getFunctionCalls() const { return m_functionCalls; }
    const std::vector<Individual>& getPopulation() const { return m_population; }

    //void selection(/* ... */);
    //void crossover(/* ... */);
    /* И другие методы, не требующие репортера */

protected:

    mu::Parser m_parser;
    double m_x, m_y;
   
    // Данные алгоритма
    const InputData* m_inputData;
    GAConfig m_config;
    GeneticEncoder m_encoder;
    std::vector<Individual> m_population;

    // Статистика
    size_t m_currentGeneration;
    size_t m_functionCalls;

    //ГСЧ
    std::mt19937 m_rnd;

    // Вспомогательные методы
    void resetAlgorithmState();
    void initializeParser(const std::string& function);
    double evaluateFitness(double x, double y);
    double worstPossibleFitness() const;
    void sortPopulation();
   
    // Генетические операторы (coming soon)
    void selection();
    void crossover();
    void mutation();
    void applyElitism();

    Result validateFunctionSyntax(const std::string& function);
};

} // namespace GA

#endif // GENETICALGORITHM_GENETICBASE_HPP_
