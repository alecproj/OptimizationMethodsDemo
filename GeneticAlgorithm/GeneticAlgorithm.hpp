//
// Created on 23 Nov, 2025
//  by alecproj
//

#ifndef GENETICALGORITHM_GENETICALGORITHM_HPP_
#define GENETICALGORITHM_GENETICALGORITHM_HPP_

#include "GeneticBase.hpp"
#include <glog/logging.h>
#include <cmath>

#ifndef DEBUG
#define DEBUG 0
#endif

namespace GA {

static inline void LOGERR(Result result)
{
    static constexpr auto ERR_STRING{ "Ошибка: " };
    LOG(ERROR) << ERR_STRING << resultToString(result);
}

template<typename Reporter>
class GeneticAlgorithm : public GeneticBase {

public:

    GeneticAlgorithm(Reporter* reporter) :
        //m_inputData{ nullptr },
        //m_parser{},
        //m_x{0.0},
        //m_y{0.0},
        //m_function_calls{ 0 },
        //m_generations{ 0 },
        m_reporter{ reporter },
        m_digitResultPrecision{0},
        m_digitComputationPrecision{0},
        m_computationPrecision{0.},
        m_resultPrecision{0.}
    {
        resetAlgorithmState();
    }

    Result setInputData(const InputData* data, const GAConfig* config = nullptr)
    {
        Result rv = Result::Fail;

        if (!data) {
            rv = Result::InvalidInput;
            LOGERR(rv);
            return rv;
        }

        // ВАЛИДАЦИЯ ВХОДНЫХ ДАННЫХ
        // Проверка функции
        if (data->function.empty()) {
            rv = Result::EmptyFunction;
            LOGERR(rv);
            return rv;
        }

        // Проверка синтаксиса функции
        Result syntax_check = validateFunctionSyntax(data->function);
        if (syntax_check != Result::Success) {
            rv = Result::ParseError;
            LOGERR(rv);
            return rv;
        }

        // Проверка типа экстремума
        if (data->extremum_type != ExtremumType::MINIMUM &&
            data->extremum_type != ExtremumType::MAXIMUM) {
            rv = Result::InvalidExtremumType;
            LOGERR(rv);
            return rv;
        }

        // Проверка корректности границ X
        if ((data->x_left_bound >= data->x_right_bound)) {
            rv = Result::InvalidXBound;
            LOGERR(rv);
            return rv;
        }

        // Проверка корректности границ Y
        if ((data->y_left_bound >= data->y_right_bound)) {
            rv = Result::InvalidYBound;
            LOGERR(rv);
            return rv;
        }

        // Проверка точности результата
        if (data->result_precision < 1 || data->result_precision > 15) {
            rv = Result::InvalidResultPrecision;
            LOGERR(rv);
            return rv;
        }

        // Проверка точности вычислений
        if (data->computation_precision < 1 || data->computation_precision > 15) {
            rv = Result::InvalidComputationPrecision;
            LOGERR(rv);
            return rv;
        }

        // Проверка что точность вычислений меньше точности результата
        if (data->computation_precision < data->result_precision) {
            rv = Result::InvalidLogicPrecision;
            LOGERR(rv);
            return rv;
        }

        // Сохраняем данные
        m_inputData = data;
        
        rv = initialize(data, config);
        if (rv != Result::Success) {
            LOGERR(rv);
            return rv;
        }

        LOG(INFO) << "Входные данные установлены.";
        return Result::Success;
    }

    Result solve()
    {
        Result rv = Result::Fail;
        if (!m_inputData || !m_reporter) {
            LOGERR(rv);
            return rv;
        }
        
        resetAlgorithmState();

        // Округляем результат
        m_digitComputationPrecision = m_inputData->computation_precision;
        m_digitResultPrecision = m_inputData->result_precision;
        m_computationPrecision = std::pow(10, (-m_inputData->computation_precision));
        m_resultPrecision = std::pow(10, (-m_inputData->result_precision));

        m_reporter->begin();

        try {

            initializeParser(m_inputData->function);
            initializePopulation();

            // Начинаем таблицу для отслеживания поколений
            auto generationTable = m_reporter->beginTable("Эволюция популяции",
                { "Поколение", "Лучший X", "Лучший Y", "Лучшая fitness",
                  "Средняя fitness", "Худшая fitness", "Стагнация" });

            // Сохраняем статистику начальной популяции
            double best_fitness_start = getBestFitness();
            double worst_fitness_start = m_population.empty() ? 0.0 : m_population.back().fitness;
            double avg_fitness_start = 0.0;
            for (const auto& ind : m_population) {
                avg_fitness_start += ind.fitness;
            }
            if (!m_population.empty()) {
                avg_fitness_start /= m_population.size();
            }


            // Основной цикл генетического алгоритма
            for (size_t generation = 0; generation < m_config.generations; ++generation) {
                
                // Сохраняем лучшее значение до поколения для отчета
                double fitness_before = getBestFitness();

                runGeneration();

                // Вычисляем статистику после поколения
                double best_fitness = getBestFitness();
                double worst_fitness = m_population.empty() ? 0.0 : m_population.back().fitness;
                double avg_fitness = 0.0;
                for (const auto& ind : m_population) {
                    avg_fitness += ind.fitness;
                }
                if (!m_population.empty()) {
                    avg_fitness /= m_population.size();
                }

                // Проверяем улучшение
                bool improved = (m_inputData->extremum_type == ExtremumType::MINIMUM)
                    ? (best_fitness < fitness_before)
                    : (best_fitness > fitness_before);

                // Добавляем строку в таблицу
                m_reporter->insertRow(generationTable, {
                    static_cast<long long>(generation + 1),
                    getBestX(),
                    getBestY(),
                    best_fitness,
                    avg_fitness,
                    worst_fitness,
                    static_cast<long long>(m_stagnationCounter)
                    });

                if (checkConvergence()) {
                    //LOG(INFO) << "Алгоритм сошелся досрочно на поколении " << generation;
                    m_reporter->insertMessage("✅ Алгоритм сошелся досрочно на поколении " +
                        std::to_string(generation + 1));
                    break;
                    break;
                }

                // Репортинг процесса
                //m_reporter->insertRow({})
  
            }
            // Закрываем таблицу поколений
            m_reporter->endTable(generationTable);

            // Финальный результат (с округлением)
            double best_x = roundResult(getBestX());
            double best_y = roundResult(getBestY());
            double best_fitness = roundResult(getBestFitness());

            // Финальный результат
            m_reporter->insertResult(best_x, best_y, best_fitness);
            m_reporter->insertMessage("Алгоритм успешно завершил работу");
            m_reporter->insertMessage("Поколений: " + std::to_string(m_currentGeneration));
            m_reporter->insertMessage("Вызовов функции: " + std::to_string(m_functionCalls));

        }
        catch (const mu::Parser::exception_type& e) {
            rv = Result::ParseError;
            LOGERR(rv);
            m_reporter->insertMessage("❌ Ошибка парсинга функции: " + std::string(e.GetMsg()));
        }
        catch (const std::exception& e) {
            rv = Result::ComputeError;
            LOGERR(rv);
            m_reporter->insertMessage("❌ Вычислительная ошибка: " + std::string(e.what()));
        }

        m_reporter->end();
        LOG(INFO) << "Алгоритм успешно завершил работу. Поколений: "
                  << m_currentGeneration << ", Вызовов функции: " << m_functionCalls;
        return Result::Success;
    }

    inline double roundResult(double v)
    {
        //return v;
        double factor = std::pow(10.0, m_digitResultPrecision);
        return std::round(v * factor) / factor;
    }

    void reportConvergence(const std::string& message) override {
        if (m_reporter) {
            m_reporter->insertMessage(message);
        }
        // Также логируем
        LOG(INFO) << message;
    }

private:
    //const InputData* m_inputData;
    //mu::Parser m_parser;
    //double m_x, m_y;                             
    //int m_function_calls;                         
    //int m_generations; 
    Reporter* m_reporter;
    int m_digitResultPrecision;                  // Количество знаков после запятой для результата
    int m_digitComputationPrecision;             // Количество знаков после запятой для вычислений
    double m_computationPrecision;
    double m_resultPrecision;
    
    void insertResultInfo(/* ... */) 
    {
    }
};

} // namespace GA

#endif // GENETICALGORITHM_GENETICALGORITHM_HPP_
