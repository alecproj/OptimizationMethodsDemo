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
#define DEBUG 0
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

public:
    // Методы для установки параметров PSO
    void setSwarmSize(int size) {
        if (size > 0) m_swarm_size = size;
    }

    void setInertiaWeight(double w) {
        m_inertia_weight = w;
    }

    void setCognitiveCoeff(double c1) {
        m_cognitive_coeff = c1;
    }

    void setSocialCoeff(double c2) {
        m_social_coeff = c2;
    }

    void setMaxIterations(int iterations) {
        m_max_iterations = iterations;
    }

    // Методы для получения текущих параметров (для тестирования)
    int getSwarmSize() const { return m_swarm_size; }
    double getInertiaWeight() const { return m_inertia_weight; }
    double getCognitiveCoeff() const { return m_cognitive_coeff; }
    double getSocialCoeff() const { return m_social_coeff; }
    int getMaxIterations() const { return m_max_iterations; }

    ParticleSwarm(Reporter* reporter) :
        m_inputData{ nullptr },
        m_reporter{ reporter },
        m_swarm_size(30),
        m_inertia_weight(0.7),
        m_cognitive_coeff(1.5),
        m_social_coeff(1.5),
        m_max_iterations(100),
        m_global_best_x(0),
        m_global_best_y(0),
        m_global_best_value(0),
        m_iterations(0),
        m_function_calls(0)
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
        m_computationPrecision = std::pow(10, (-m_inputData->computation_precision));
        m_resultPrecision = std::pow(10, (-m_inputData->result_precision));

        try {
            initializeParser(m_inputData->function);

            m_swarm.resize(m_swarm_size);
            initializeSwarmPositions();
            
            initializeSwarmVelocities();

            LOG(INFO) << "Тест инициализации скоростей (первые 5 частиц):";
            for (int i = 0; i < std::min(5, m_swarm_size); ++i) {
                LOG(INFO) << "Частица " << i << ": vx=" << m_swarm[i].vx << ", vy=" << m_swarm[i].vy;
            }

            // Проверка разумности скоростей (не слишком большие)
            bool reasonable_velocities = true;
            double x_range = m_inputData->x_right_bound - m_inputData->x_left_bound;
            double y_range = m_inputData->y_right_bound - m_inputData->y_left_bound;
            double vx_max = 0.1 * x_range;
            double vy_max = 0.1 * y_range;

            for (const auto& particle : m_swarm) {
                if (std::abs(particle.vx) > vx_max || std::abs(particle.vy) > vy_max) {
                    reasonable_velocities = false;
                    break;
                }
            }
            LOG(INFO) << "Скорости в разумных пределах: " << (reasonable_velocities ? "ДА" : "НЕТ");
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


private:
    void initializeSwarmPositions() {
        // Для простоты начнем с псевдослучайных чисел, потом добавим <random>
        double x_range = m_inputData->x_right_bound - m_inputData->x_left_bound;
        double y_range = m_inputData->y_right_bound - m_inputData->y_left_bound;

        for (auto& particle : m_swarm) {
            // Простой способ получить псевдослучайные числа
            double rand1 = static_cast<double>(rand()) / RAND_MAX; // 0..1
            double rand2 = static_cast<double>(rand()) / RAND_MAX; // 0..1

            particle.x = m_inputData->x_left_bound + rand1 * x_range;
            particle.y = m_inputData->y_left_bound + rand2 * y_range;

            // Инициализируем лучшие позиции текущими
            particle.best_x = particle.x;
            particle.best_y = particle.y;
        }
    }
    void initializeSwarmVelocities() {
        // Вычислим диапазоны по x и y для задания начальных скоростей
        double x_range = m_inputData->x_right_bound - m_inputData->x_left_bound;
        double y_range = m_inputData->y_right_bound - m_inputData->y_left_bound;

        // Начальные скорости зададим в пределах [-v_max, v_max], где v_max = 0.1 * диапазон
        double vx_max = 0.1 * x_range;
        double vy_max = 0.1 * y_range;

        for (auto& particle : m_swarm) {
            // Генерируем случайные скорости в заданном диапазоне
            double rand1 = static_cast<double>(rand()) / RAND_MAX; // 0..1
            double rand2 = static_cast<double>(rand()) / RAND_MAX; // 0..1

            particle.vx = -vx_max + rand1 * 2 * vx_max; // от -vx_max до vx_max
            particle.vy = -vy_max + rand2 * 2 * vy_max;
        }
    }

    struct ParticleData {
        double x, y;           // Текущая позиция
        double vx, vy;         // Текущая скорость
        double best_x, best_y; // Лучшая найденная позиция
        double best_value;     // Лучшее значение функции

        ParticleData() : x(0), y(0), vx(0), vy(0),
                        best_x(0), best_y(0), best_value(0) {}
    };

    // Параметры алгоритма PSO
    int m_swarm_size;                    // Размер роя
    double m_inertia_weight;             // Инерционный вес (w)
    double m_cognitive_coeff;            // Когнитивный коэффициент (c1)
    double m_social_coeff;               // Социальный коэффициент (c2)
    int m_max_iterations;                // Максимальное число итераций

    // Состояние алгоритма
    std::vector<ParticleData> m_swarm;
    double m_global_best_x, m_global_best_y, m_global_best_value;
    int m_iterations;
    int m_function_calls;

    void insertResultInfo(/* ... */) 
    {
    }
};

} // namespace GA

#endif // PARTICLESWARM_PARTICLESWARM_HPP_
