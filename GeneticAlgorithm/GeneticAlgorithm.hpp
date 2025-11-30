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
        m_inputData{ nullptr },
        m_reporter{ reporter },
        m_parser{},
        m_x{0.0},
        m_y{0.0},
        m_function_calls{0},
        m_generations{0},
        m_digitResultPrecision{0},
        m_digitComputationPrecision{0},
        m_computationPrecision{0.},
        m_resultPrecision{0.}
    {
        resetAlgorithmState();
    }

    Result setInputData(const InputData* data)
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
        
        rv = initialize(data);
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
        m_computationPrecision = std::pow(
            10, (-m_inputData->computation_precision));
        m_resultPrecision = std::pow(
            10, (-m_inputData->result_precision));

        m_reporter->begin();

        try {

            initializeParser(m_inputData->function);

            // Основной цикл генетического алгоритма
            for (size_t generation = 0; generation < m_config.generations; ++generation) {
                runGeneration();

                // Репортинг процесса
                //m_reporter->insertRow({})
  
            }
            // Финальный результат (с округлением)
            double best_x = getBestX;
            double best_y = getBestY;
            double best_fitness = getBestFitness();

             // Финальный результат c округлением добавить
             //m_reporter->insertResult(best_x, best_y, best_fitness);
        }
        catch (const mu::Parser::exception_type& e) {
            rv = Result::ParseError;
            LOGERR(rv);
        }
        catch (const std::exception& e) {
            rv = Result::ComputeError;
            LOGERR(rv);
        }

        //m_reporter->insertResult(0, 0, 0);
        m_reporter->end();
        LOG(INFO) << "Алгоритм успешно завершил работу.";
        return Result::Success;
    }

private:

    const InputData* m_inputData;
    Reporter* m_reporter;
    mu::Parser m_parser;
    double m_x, m_y;                 // Текущие переменные для парсера
    int m_function_calls;
    int m_generations;               // Количество поколений (итераций)
    int m_digitResultPrecision;      // Количество знаков после запятой для результата
    int m_digitComputationPrecision; // Количество знаков после запятой для вычислений
    double m_computationPrecision;
    double m_resultPrecision;

    void insertResultInfo(/* ... */) 
    {
    }
};

} // namespace GA

#endif // GENETICALGORITHM_GENETICALGORITHM_HPP_
