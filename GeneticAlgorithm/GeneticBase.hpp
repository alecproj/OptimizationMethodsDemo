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
#include <glog/logging.h>

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
    //Result checkFunction(const std::string &function);

    // == Сходимость ===

    // Метод проверки сходимости
    bool checkConvergence();

    // Проверка генетического разнообразия
    bool hasLowDiversity() const;

    // Геттеры
    double getBestX()         const { return m_historicalBest.x; }
    double getBestY()         const { return m_historicalBest.y; }
    double getBestFitness()   const { return m_historicalBest.fitness; }
    size_t getIteraionts()    const { return m_currentGeneration; }
    size_t getFunctionCalls() const { return m_functionCalls; }
    const std::vector<Individual>& getPopulation() const { return m_population; }


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

    // ГСЧ
    std::mt19937 m_rnd;

    // Для отслеживания сходимости
    double m_bestFitnessHistory;                 // Лучшее приспособление предыдущего поколения
    size_t m_stagnationCounter;                  // Счётчик поколений без улучшений
    static constexpr size_t MAX_STAGNATION = 50; // Максимум поколений без улучшений
    Individual m_historicalBest;  

    virtual void reportConvergence(const std::string& message) {
        LOG(INFO) << message;
    }

    // Вспомогательные методы
    void resetAlgorithmState();
    void initializeParser(const std::string& function);
    double evaluateFitness(double x, double y);
    double worstPossibleFitness() const;
    void sortPopulation();
    // Сброс отслеживания сходимости
    void resetConvergenceTracking();

    void updateHistoricalBest();
   
    // === Генетические операторы =====

    void selection(); // Турнирная селекция для выявления наилучших будущих родителей
    void crossover(); // Кроссовер: Скрещивание родителей для создания потомков со смешанными генами
    void mutation();  // Мутация: внесения случайных изменений в генах особей для поддержания генетического разнообразия

    // Копирование наилучших особей из текущего поколения в следующее поколение
    void applyElitism(std::vector<Individual>& newPopulation);

    Result validateFunctionSyntax(const std::string& function);
};

} // namespace GA

#endif // GENETICALGORITHM_GENETICBASE_HPP_
