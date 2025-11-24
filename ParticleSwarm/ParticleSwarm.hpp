//
// Created on 23 Nov, 2025
//  by alecproj
//

#ifndef PARTICLESWARM_PARTICLESWARM_HPP_
#define PARTICLESWARM_PARTICLESWARM_HPP_

#include "ParticleBase.hpp"
#include <glog/logging.h>
#include <cmath>

#ifndef DEBUG
#define DEBUG 1
#endif

namespace PS {

static inline void LOGERR(Result result)
{
    static constexpr auto ERR_STRING{ "Ошибка: " };
    LOG(ERROR) << ERR_STRING << resultToString(result);
}

template<typename Reporter>
class ParticleSwarm : public ParticleBase {

    static constexpr double CONST_EXAMPLE{ 0 };

public:

    ParticleSwarm(Reporter* reporter) :
        m_inputData{ nullptr },
        m_reporter{ reporter }
    {
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

        // Проверка на дифференцируемость
        rv = checkFunctionDifferentiability(data->function);
        if (rv != Result::Success) {
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
        m_computationDigits = m_inputData->computation_precision;
        m_resultDigits = m_inputData->result_precision;
        m_computationPrecision = std::pow(
            10, (-m_inputData->computation_precision));
        m_resultPrecision = std::pow(
            10, (-m_inputData->result_precision));

        try {
            initializeParser(m_inputData->function);

            // ...
        }
        catch (const mu::Parser::exception_type& e) {
            rv = Result::ParseError;
            LOGERR(rv);
        }
        catch (const std::exception& e) {
            rv = Result::ComputeError;
            LOGERR(rv);
        }

        m_reporter->insertResult(0, 0, 0);
        LOG(INFO) << "Алгоритм успешно завершил работу.";
        return Result::Success;
    }

private:
    const InputData* m_inputData;
    Reporter* m_reporter;

    void insertResultInfo(/* ... */) 
    {
    }
};

} // namespace GA

#endif // PARTICLESWARM_PARTICLESWARM_HPP_
