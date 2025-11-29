//
// Created on 23 Nov, 2025
//  by alecproj
//

#ifndef PARTICLESWARM_PARTICLESWARM_HPP_
#define PARTICLESWARM_PARTICLESWARM_HPP_

#include "ParticleBase.hpp"
#include <glog/logging.h>
#include <cmath>
#include <limits>
#include <vector>
#include <algorithm>

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

    // Методы для получения текущих параметров
    int getSwarmSize() const { return m_swarm_size; }
    double getInertiaWeight() const { return m_inertia_weight; }
    double getCognitiveCoeff() const { return m_cognitive_coeff; }
    double getSocialCoeff() const { return m_social_coeff; }
    int getMaxIterations() const { return m_max_iterations; }

    // Методы для получения результатов
    double getX() const { return m_global_best_x; }
    double getY() const { return m_global_best_y; }
    double getOptimumValue() const { return m_global_best_value; }
    int getIterations() const { return m_iterations; }
    int getFunctionCalls() const { return m_function_calls; }

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
        if (data->function.empty()) {
            rv = Result::EmptyFunction;
            LOGERR(rv);
            return rv;
        }

        Result syntax_check = validateFunctionSyntax(data->function);
        if (syntax_check != Result::Success) {
            rv = Result::ParseError;
            LOGERR(rv);
            return rv;
        }

        rv = checkFunctionDifferentiability(data->function);
        if (rv != Result::Success) {
            LOGERR(rv);
            return rv;
        }

        if (data->extremum_type != ExtremumType::MINIMUM &&
            data->extremum_type != ExtremumType::MAXIMUM) {
            rv = Result::InvalidExtremumType;
            LOGERR(rv);
            return rv;
        }

        if ((data->x_left_bound >= data->x_right_bound)) {
            rv = Result::InvalidXBound;
            LOGERR(rv);
            return rv;
        }

        if ((data->y_left_bound >= data->y_right_bound)) {
            rv = Result::InvalidYBound;
            LOGERR(rv);
            return rv;
        }

        if (data->result_precision < 1 || data->result_precision > 15) {
            rv = Result::InvalidResultPrecision;
            LOGERR(rv);
            return rv;
        }

        if (data->computation_precision < 1 || data->computation_precision > 15) {
            rv = Result::InvalidComputationPrecision;
            LOGERR(rv);
            return rv;
        }

        if (data->computation_precision < data->result_precision) {
            rv = Result::InvalidLogicPrecision;
            LOGERR(rv);
            return rv;
        }

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

            LOG(INFO) << "Функция для оптимизации: " << m_inputData->function;

            // Инициализация роя
            m_swarm.resize(m_swarm_size);
            initializeSwarmPositions();
            initializeSwarmVelocities();
            initializeBestValues();

            LOG(INFO) << "Начальный глобальный лучший: (" << m_global_best_x
                      << ", " << m_global_best_y << ") значение = " << m_global_best_value;

            // Основной цикл алгоритма PSO
            int stagnation_count = 0;
            double previous_global_best = m_global_best_value;

            for (m_iterations = 1; m_iterations <= m_max_iterations; ++m_iterations) {
                // Сохраняем предыдущее лучшее значение для проверки сходимости
                previous_global_best = m_global_best_value;

                // Основные шаги PSO
                updateSwarmVelocities();
                updateSwarmPositions();
                updateBestValues();

                // Проверка сходимости
                double improvement = std::abs(m_global_best_value - previous_global_best);
                if (improvement < m_resultPrecision) {
                    stagnation_count++;
                } else {
                    stagnation_count = 0;
                }

                // Вывод прогресса каждые 10 итераций
                if (m_iterations % 10 == 0) {
                    LOG(INFO) << "Итерация " << m_iterations
                              << ": Лучшее значение = " << m_global_best_value
                              << " в точке (" << m_global_best_x << ", " << m_global_best_y << ")";
                }

                // Критерии остановки
                if (stagnation_count >= 10) { // 10 итераций без улучшения
                    LOG(INFO) << "Алгоритм сошелся после " << m_iterations << " итераций";
                    break;
                }
            }

            if (m_iterations > m_max_iterations) {
                LOG(INFO) << "Достигнуто максимальное количество итераций: " << m_max_iterations;
            }

            LOG(INFO) << "Финальный результат:";
            LOG(INFO) << "Точка оптимума: (" << m_global_best_x << ", " << m_global_best_y << ")";
            LOG(INFO) << "Значение функции: " << m_global_best_value;
            LOG(INFO) << "Всего итераций: " << m_iterations;
            LOG(INFO) << "Всего вызовов функции: " << m_function_calls;

        }
        catch (const mu::Parser::exception_type& e) {
            rv = Result::ParseError;
            LOGERR(rv);
        }
        catch (const std::exception& e) {
            rv = Result::ComputeError;
            LOGERR(rv);
        }

        m_reporter->insertResult(m_global_best_x, m_global_best_y, m_global_best_value);
        LOG(INFO) << "Алгоритм успешно завершил работу.";
        return Result::Success;
    }

private:
    const InputData* m_inputData;
    Reporter* m_reporter;

    struct ParticleData {
        double x, y;           // Текущая позиция
        double vx, vy;         // Текущая скорость
        double best_x, best_y; // Лучшая найденная позиция
        double best_value;     // Лучшее значение функции

        ParticleData() : x(0), y(0), vx(0), vy(0),
                        best_x(0), best_y(0), best_value(0) {}
    };

    // Параметры алгоритма PSO
    int m_swarm_size;
    double m_inertia_weight;
    double m_cognitive_coeff;
    double m_social_coeff;
    int m_max_iterations;

    // Состояние алгоритма
    std::vector<ParticleData> m_swarm;
    double m_global_best_x, m_global_best_y, m_global_best_value;
    int m_iterations;
    int m_function_calls;

    // Вспомогательные методы
    bool isBetter(double new_value, double current_best) const {
        if (m_inputData->extremum_type == ExtremumType::MINIMUM) {
            return new_value < current_best;
        } else {
            return new_value > current_best;
        }
    }

    void initializeSwarmPositions() {
        double x_range = m_inputData->x_right_bound - m_inputData->x_left_bound;
        double y_range = m_inputData->y_right_bound - m_inputData->y_left_bound;

        for (auto& particle : m_swarm) {
            double rand1 = static_cast<double>(rand()) / RAND_MAX;
            double rand2 = static_cast<double>(rand()) / RAND_MAX;

            particle.x = m_inputData->x_left_bound + rand1 * x_range;
            particle.y = m_inputData->y_left_bound + rand2 * y_range;

            particle.best_x = particle.x;
            particle.best_y = particle.y;
        }
    }

    void initializeSwarmVelocities() {
        double x_range = m_inputData->x_right_bound - m_inputData->x_left_bound;
        double y_range = m_inputData->y_right_bound - m_inputData->y_left_bound;

        double vx_max = 0.1 * x_range;
        double vy_max = 0.1 * y_range;

        for (auto& particle : m_swarm) {
            double rand1 = static_cast<double>(rand()) / RAND_MAX;
            double rand2 = static_cast<double>(rand()) / RAND_MAX;

            particle.vx = -vx_max + rand1 * 2 * vx_max;
            particle.vy = -vy_max + rand2 * 2 * vy_max;
        }
    }

    void initializeBestValues() {
        m_global_best_value = (m_inputData->extremum_type == ExtremumType::MINIMUM)
            ? std::numeric_limits<double>::max()
            : std::numeric_limits<double>::lowest();

        for (auto& particle : m_swarm) {
            double value = evaluateFunction(particle.x, particle.y);
            m_function_calls++;

            particle.best_value = value;

            if (isBetter(value, m_global_best_value)) {
                m_global_best_value = value;
                m_global_best_x = particle.x;
                m_global_best_y = particle.y;
            }
        }
    }

    void updateParticleVelocity(ParticleData& particle) {
        double r1 = static_cast<double>(rand()) / RAND_MAX;
        double r2 = static_cast<double>(rand()) / RAND_MAX;

        particle.vx = m_inertia_weight * particle.vx +
                     m_cognitive_coeff * r1 * (particle.best_x - particle.x) +
                     m_social_coeff * r2 * (m_global_best_x - particle.x);

        particle.vy = m_inertia_weight * particle.vy +
                     m_cognitive_coeff * r1 * (particle.best_y - particle.y) +
                     m_social_coeff * r2 * (m_global_best_y - particle.y);

        // Ограничение скорости
        double v_max = 0.2 * (m_inputData->x_right_bound - m_inputData->x_left_bound);
        particle.vx = std::max(std::min(particle.vx, v_max), -v_max);
        particle.vy = std::max(std::min(particle.vy, v_max), -v_max);
    }

    void updateSwarmVelocities() {
        for (auto& particle : m_swarm) {
            updateParticleVelocity(particle);
        }
    }

    void updateParticlePosition(ParticleData& particle) {
        particle.x += particle.vx;
        particle.y += particle.vy;

        // Обработка границ
        if (particle.x < m_inputData->x_left_bound) {
            particle.x = m_inputData->x_left_bound;
            particle.vx = -particle.vx * 0.5;
        } else if (particle.x > m_inputData->x_right_bound) {
            particle.x = m_inputData->x_right_bound;
            particle.vx = -particle.vx * 0.5;
        }

        if (particle.y < m_inputData->y_left_bound) {
            particle.y = m_inputData->y_left_bound;
            particle.vy = -particle.vy * 0.5;
        } else if (particle.y > m_inputData->y_right_bound) {
            particle.y = m_inputData->y_right_bound;
            particle.vy = -particle.vy * 0.5;
        }
    }

    void updateSwarmPositions() {
        for (auto& particle : m_swarm) {
            updateParticlePosition(particle);
        }
    }

    void updateParticleBest(ParticleData& particle) {
        double current_value = evaluateFunction(particle.x, particle.y);
        m_function_calls++;

        if (isBetter(current_value, particle.best_value)) {
            particle.best_value = current_value;
            particle.best_x = particle.x;
            particle.best_y = particle.y;

            if (isBetter(current_value, m_global_best_value)) {
                m_global_best_value = current_value;
                m_global_best_x = particle.x;
                m_global_best_y = particle.y;
            }
        }
    }

    void updateBestValues() {
        for (auto& particle : m_swarm) {
            updateParticleBest(particle);
        }
    }

    void insertResultInfo(/* ... */)
    {
    }
};

} // namespace PS

#endif // PARTICLESWARM_PARTICLESWARM_HPP_