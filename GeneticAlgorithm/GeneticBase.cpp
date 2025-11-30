//
// Created on 23 Nov, 2025
//  by alecproj
//

#include "GeneticBase.hpp"
#include <glog/logging.h>
#include <cstddef>

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

    // Создание популяции
    void GeneticBase::initializePopulation()
    {
        // Очищаем мусор
        m_population.clear();
        // Выделяем память под популяцию
        m_population.reserve(m_config.population_size);

        // Создаём случайных особей
        for (size_t i = 0; i < m_config.population_size; ++i) {

            Individual individ = m_encoder.createRandomIndividual();
            // Вычисляем fitness: f(x, y) для каждой особи
            individ.fitness = evaluateFitness(individ.x, individ.y);
            // Добавляем особь в популяцию
            m_population.push_back(individ);
        }

        // Сортируем популяцию по fitness
        sortPopulation();

        LOG(INFO) << "Популяция инициализирована. Размер: " << m_population.size();
        LOG(INFO) << "Лучшая fitness: " << m_population[0].fitness
            << " в точке (" << m_population[0].x << ", " << m_population[0].y << ")";
    }
    
    // Копирование наилучших особей из текущего поколения в следующее поколение
    void GeneticBase::applyElitism(std::vector<Individual>& newPopulation)
    {
        // Копируем лучших особей из текущего поколения
        for (size_t i = 0; i < m_config.elite_count && i < m_population.size(); ++i) {
            if (newPopulation.size() < m_config.population_size) {
                newPopulation.push_back(m_population[i]);
            }
        }
    }

    // Турнирная селекция для выявления наилучших будущих родителей
    void GeneticBase::selection()
    {
        std::vector<Individual> selected;
        
        // Выделяем память под будущих родителей
        selected.reserve(m_config.population_size);

        // Турнирная селекция (диапазон выбора особей из всей популяции для сражения на турнире)
        std::uniform_int_distribution<size_t> dist(0, m_population.size() - 1);
        
        for (size_t i = 0; i < m_config.population_size; ++i) {
            
            // Выбираем двух случайных особей для турнира
            size_t index1 = dist(m_rnd);
            size_t index2 = dist(m_rnd);

            // Получаем их характеристики
            const Individual& candidate1 = m_population[index1];
            const Individual& candidate2 = m_population[index2];

            // Выбираем лучшего будущего родителя от наших условий
            if (m_inputData->extremum_type == ExtremumType::MAXIMUM) {

                // Для максимума выбираем будущего родителя с большей приспособленностью
                selected.push_back(candidate1.fitness > candidate2.fitness ? candidate1 : candidate2);
            } else {
               
                // Для минимума выбираем будущего родителя с меньшей приспособленностью
                selected.push_back(candidate1.fitness < candidate2.fitness ? candidate1 : candidate2);
            }
        }
        // Умный перенос особей: когда особь перебрасывается из A в B, то из A она удаляется и появляется в B (смена указателей)
        m_population = std::move(selected);
        
        // Теперь популяция содержит отобранных будущих родителей, которые отважно боролись на турнире, для кроссовера
        // Некоторые особи могут дублироваться, что характерно для селекции
    }

    // Кроссовер: Скрещивание родителей для создания потомков со смешанными генами
    void GeneticBase::crossover()
    {
        // Список потомков
        std::vector<Individual> offspring;
        // Выделение памяти под потомков
        offspring.reserve(m_config.population_size);

        // Случайная вероятность происхождения кроссовера
        std::uniform_real_distribution<double> prob_dist(0.0, 1.0);
        // Точка разрыва хромосомы для обмены генами
        std::uniform_int_distribution<size_t> crossover_point_dist(1, m_encoder.getChromosomeLength() - 2);

        // Проходим по популяции парами
        for (size_t i = 0; i < m_population.size() - 1; i += 2) {
            
            const Individual& parent1 = m_population[i];
            const Individual& parent2 = m_population[i + 1];

            // Проверка: произойдёт ли кроссовер
            if (prob_dist(m_rnd) < m_config.crossover_rate) {

                // Выбираем точку разрыва хромосомы для обмены генами
                size_t crossover_point = crossover_point_dist(m_rnd);

                // Создаём хромосомы потомков
                std::vector<bool> child1_chromosome, child2_chromosome;
                child1_chromosome.reserve(m_encoder.getChromosomeLength());
                child2_chromosome.reserve(m_encoder.getChromosomeLength());

                // Первый потомок: первая часть генов от parent1, вторая часть генов от parent2
                child1_chromosome.insert(child1_chromosome.end(),
                                         parent1.chromosome.begin(),
                                         parent1.chromosome.begin() + crossover_point);
                child1_chromosome.insert(child1_chromosome.end(),
                                         parent2.chromosome.begin() + crossover_point,
                                         parent2.chromosome.end());
                
                // Второй потомок: первая часть генов от parent2, вторая часть генов от parent1
                child2_chromosome.insert(child2_chromosome.end(),
                                         parent2.chromosome.begin(),
                                         parent2.chromosome.begin() + crossover_point);
                child2_chromosome.insert(child2_chromosome.end(),
                                         parent1.chromosome.begin() + crossover_point,
                                         parent1.chromosome.end());

                // Создаём потомков с изменёнными хромосомами
                Individual child1 = m_encoder.decode(child1_chromosome);
                Individual child2 = m_encoder.decode(child2_chromosome);

                // Вычисляем приспособленность f(x, y) потомков
                child1.fitness = evaluateFitness(child1.x, child1.y);
                child2.fitness = evaluateFitness(child2.x, child2.y);

                // Добавляем потомков в список
                offspring.push_back(child1);
                offspring.push_back(child2);

            } else {

                // Если кроссовер не произошёл, то просто добавляем изначальных родителей
                offspring.push_back(parent1);
                offspring.push_back(parent2);
            }
        }
        // Проверка на нечётное количество потомков: добавляем последнего родителя
        if (offspring.size() < m_config.population_size && !m_population.empty()) {
            offspring.push_back(m_population.back());
        }

        m_population = std::move(offspring);
        // Теперь популяции содержит потомков с новыми генами от родителей
    }

    // Мутация: внесения случайных изменений в генах особей для поддержания генетического разнообразия
    void GeneticBase::mutation()
    {
        // Случайная вероятность мутации
        std::uniform_real_distribution<double> prob_dist(0.0, 1.0);
        
        // Проходим по всем особям из популяции
        for (Individual& individual : m_population) {
            // Поточная мутация: проверяем каждый бит
            for (size_t i = 0; i < individual.chromosome.size(); ++i) {
                if (prob_dist(m_rnd) < m_config.mutation_rate) {
                    individual.chromosome[i] = !individual.chromosome[i];
                }
            }

            // Получаем новую мутирующую особь
            Individual mutated = m_encoder.decode(individual.chromosome);
            individual.x = mutated.x;
            individual.y = mutated.y;
            individual.fitness = evaluateFitness(individual.x, individual.y);
        }
    }
    

} // namespace GA
