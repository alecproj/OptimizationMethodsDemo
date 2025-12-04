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

        //LOG(INFO) << "Функция для оптимизации: " << m_inputData->function;

        // ============ НАЧАЛО РЕПОРТЕРА ============
        m_reporter->insertMessage("=== АЛГОРИТМ РОЯ ЧАСТИЦ (PSO) ===");
        m_reporter->insertMessage("Начало работы алгоритма");
        //m_reporter->insertMessage("Цель: " + (m_inputData->extremum_type == ExtremumType::MINIMUM ? "Поиск минимума" : "Поиск максимума"));
        m_reporter->insertMessage("Функция: " + m_inputData->function);

        // Таблица параметров алгоритма
        int paramsTableId = m_reporter->beginTable("Параметры алгоритма",
            {"Параметр", "Значение"});

        m_reporter->insertRow(paramsTableId, {
            "Размер роя", static_cast<long long>(m_swarm_size)
        });
        m_reporter->insertRow(paramsTableId, {
            "Инерция", m_inertia_weight
        });
        m_reporter->insertRow(paramsTableId, {
            "Когнитивный коэффициент", m_cognitive_coeff
        });
        m_reporter->insertRow(paramsTableId, {
            "Социальный коэффициент", m_social_coeff
        });
        m_reporter->insertRow(paramsTableId, {
            "Макс. итераций", static_cast<long long>(m_max_iterations)
        });

        m_reporter->endTable(paramsTableId);

        // Таблица границ области
        /*
        int boundsTableId = m_reporter->beginTable("Границы области поиска",
            {"Ось", "Левая граница", "Правая граница"});

        m_reporter->insertRow(boundsTableId, {
            "X", m_inputData->x_left_bound, m_inputData->x_right_bound
        });
        m_reporter->insertRow(boundsTableId, {
            "Y", m_inputData->y_left_bound, m_inputData->y_right_bound
        });

        m_reporter->endTable(boundsTableId);
        */
        // ==========================================

        // Инициализация роя
        m_reporter->insertMessage("=== ИНИЦИАЛИЗАЦИЯ РОЯ ===");
        m_swarm.resize(m_swarm_size);
        initializeSwarmPositions();
        initializeSwarmVelocities();
        initializeBestValues();

        m_reporter->insertMessage("Начальный глобальный лучший:");
        m_reporter->insertValue("X", m_global_best_x);
        m_reporter->insertValue("Y", m_global_best_y);
        m_reporter->insertValue("Значение функции", m_global_best_value);
        m_reporter->insertValue("Вызовов функции", static_cast<double>(m_function_calls));

        //LOG(INFO) << "Начальный глобальный лучший: (" << m_global_best_x << ", " << m_global_best_y << ") значение = " << m_global_best_value;

        // Основной цикл алгоритма PSO
        int stagnation_count = 0;
        double previous_global_best = m_global_best_value;

        // Таблица для всех итераций
        int iterationsTableId = m_reporter->beginTable("Ход выполнения алгоритма",
            {"Итерация", "Лучш.X", "Лучш.Y", "Значение", "Улучшение", "Вызовов", "Стагнация"});

        for (m_iterations = 1; m_iterations <= m_max_iterations; ++m_iterations) {
            // Сохраняем предыдущее лучшее значение для проверки сходимости
            previous_global_best = m_global_best_value;

            // Основные шаги PSO
            updateSwarmVelocities();
            updateSwarmPositions();
            updateBestValues();

            // ============ ДОБАВЛЕНИЕ СТРОКИ В ТАБЛИЦУ КАЖДОЙ ИТЕРАЦИИ ============
            // Проверка сходимости
            double improvement = std::abs(m_global_best_value - previous_global_best);
            if (improvement < m_resultPrecision) {
                stagnation_count++;
            } else {
                stagnation_count = 0;
            }

            // Добавляем строку для текущей итерации
            m_reporter->insertRow(iterationsTableId, {
                static_cast<long long>(m_iterations),
                m_global_best_x,
                m_global_best_y,
                m_global_best_value,
                improvement,
                static_cast<double>(m_function_calls),
                static_cast<double>(stagnation_count)
            });

            // ============ ДЕТАЛЬНАЯ ИНФОРМАЦИЯ КАЖДЫЕ 10 ИТЕРАЦИЙ ============
            /*
            if (m_iterations % 10 == 0) {
                // Таблица статистики частиц на этой итерации
                int detailTableId = m_reporter->beginTable(
                    "Детали итерации " + std::to_string(m_iterations),
                    {"Статистика", "Значение", "Единицы"});

                // Вычисляем статистику
                double min_x = m_swarm[0].x, max_x = m_swarm[0].x;
                double min_y = m_swarm[0].y, max_y = m_swarm[0].y;
                double avg_x = 0, avg_y = 0, avg_v = 0;

                for (const auto& particle : m_swarm) {
                    min_x = std::min(min_x, particle.x);
                    max_x = std::max(max_x, particle.x);
                    min_y = std::min(min_y, particle.y);
                    max_y = std::max(max_y, particle.y);
                    avg_x += particle.x;
                    avg_y += particle.y;
                    avg_v += std::sqrt(particle.vx * particle.vx + particle.vy * particle.vy);
                }
                avg_x /= m_swarm_size;
                avg_y /= m_swarm_size;
                avg_v /= m_swarm_size;

                // Вычисляем разброс
                double spread_x = 0, spread_y = 0;
                for (const auto& particle : m_swarm) {
                    spread_x += std::pow(particle.x - avg_x, 2);
                    spread_y += std::pow(particle.y - avg_y, 2);
                }
                spread_x = std::sqrt(spread_x / m_swarm_size);
                spread_y = std::sqrt(spread_y / m_swarm_size);

                // Добавляем статистику
                m_reporter->insertRow(detailTableId, {
                    "Лучшая частица",
                    m_global_best_value,
                    "значение f(x,y)"
                });
                m_reporter->insertRow(detailTableId, {
                    "Диапазон X",
                    max_x - min_x,
                    "разность"
                });
                m_reporter->insertRow(detailTableId, {
                    "Диапазон Y",
                    max_y - min_y,
                    "разность"
                });
                m_reporter->insertRow(detailTableId, {
                    "Средняя скорость",
                    avg_v,
                    "модуль"
                });
                m_reporter->insertRow(detailTableId, {
                    "Разброс X",
                    spread_x,
                    "СКО"
                });
                m_reporter->insertRow(detailTableId, {
                    "Разброс Y",
                    spread_y,
                    "СКО"
                });
                m_reporter->insertRow(detailTableId, {
                    "Частиц вблизи оптимума",
                    countParticlesNearOptimum(0.1),
                    "штук"
                });

                m_reporter->endTable(detailTableId);

                m_reporter->insertMessage("Прогресс: " + std::to_string(m_iterations) +
                    "/" + std::to_string(m_max_iterations) + " итераций");
            }
            */
            //LOG(INFO) << "Итерация " << m_iterations << ": Лучшее значение = " << m_global_best_value << " в точке (" << m_global_best_x << ", " << m_global_best_y << ")";

            // Критерии остановки
            if (stagnation_count >= 10) { // 10 итераций без улучшения
                m_reporter->insertMessage("=== АЛГОРИТМ СОШЕЛСЯ ===");
                m_reporter->insertValue("Итераций выполнено", static_cast<double>(m_iterations));
                m_reporter->insertValue("Стагнация", static_cast<double>(stagnation_count));
                m_reporter->insertValue("Финальное улучшение", improvement);
                break;
            }
        }

        m_reporter->endTable(iterationsTableId);

        if (m_iterations > m_max_iterations) {
            //LOG(INFO) << "Достигнуто максимальное количество итераций: " << m_max_iterations;
            m_reporter->insertMessage("=== ДОСТИГНУТ МАКСИМУМ ИТЕРАЦИЙ ===");
            m_reporter->insertMessage("Остановка по достижению " +
                std::to_string(m_max_iterations) + " итераций");
        }

        /*
        LOG(INFO) << "Финальный результат:";
        LOG(INFO) << "Точка оптимума: (" << m_global_best_x << ", " << m_global_best_y << ")";
        LOG(INFO) << "Значение функции: " << m_global_best_value;
        LOG(INFO) << "Всего итераций: " << m_iterations;
        LOG(INFO) << "Всего вызовов функции: " << m_function_calls;
        */

        // ============ ФИНАЛЬНЫЙ ОТЧЕТ ============
        m_reporter->insertMessage("=== ФИНАЛЬНЫЙ ОТЧЕТ ===");

        // Итоговая таблица
        int finalTableId = m_reporter->beginTable("Результаты оптимизации",
            {"Параметр", "Значение"});

        m_reporter->insertRow(finalTableId, {
            "Найденный X", m_global_best_x
        });
        m_reporter->insertRow(finalTableId, {
            "Найденный Y", m_global_best_y
        });
        m_reporter->insertRow(finalTableId, {
            "Значение f(x,y)", m_global_best_value
        });
        m_reporter->insertRow(finalTableId, {
            "Итераций выполнено", static_cast<long long>(m_iterations)
        });
        m_reporter->insertRow(finalTableId, {
            "Вызовов функции", static_cast<long long>(m_function_calls)
        });
        m_reporter->insertRow(finalTableId, {
            "Скорость сходимости",
            static_cast<double>(m_function_calls) / std::max(1, m_iterations)
        });
        m_reporter->insertRow(finalTableId, {
            "Точность результата", m_resultPrecision
        });

        m_reporter->endTable(finalTableId);

        m_reporter->insertMessage("Алгоритм успешно завершен!");
        // =========================================

    }
    catch (const mu::Parser::exception_type& e) {
        rv = Result::ParseError;
        LOGERR(rv);
        m_reporter->insertMessage("ОШИБКА ПАРСИНГА: " + std::string(e.GetMsg()));
    }
    catch (const std::exception& e) {
        rv = Result::ComputeError;
        LOGERR(rv);
        m_reporter->insertMessage("ОШИБКА ВЫЧИСЛЕНИЙ: " + std::string(e.what()));
    }

    m_reporter->insertResult(m_global_best_x, m_global_best_y, m_global_best_value);
    m_reporter->insertMessage("Алгоритм успешно завершен!");
    //LOG(INFO) << "Алгоритм успешно завершил работу.";
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
    double m_velocity_limit_factor = 0.1;

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

        double vx_max = m_velocity_limit_factor * x_range;
        double vy_max = m_velocity_limit_factor * y_range;

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


    // Вспомогательный метод для подсчета частиц вблизи оптимума
    int countParticlesNearOptimum(double radius_factor) const {
        int count = 0;
        double x_range = m_inputData->x_right_bound - m_inputData->x_left_bound;
        double y_range = m_inputData->y_right_bound - m_inputData->y_left_bound;
        double radius = radius_factor * std::sqrt(x_range * x_range + y_range * y_range);

        for (const auto& particle : m_swarm) {
            double distance = std::sqrt(
                std::pow(particle.x - m_global_best_x, 2) +
                std::pow(particle.y - m_global_best_y, 2)
            );
            if (distance < radius) {
                count++;
            }
        }
        return count;
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
        double vx_max = m_velocity_limit_factor * (m_inputData->x_right_bound - m_inputData->x_left_bound);
        double vy_max = m_velocity_limit_factor * (m_inputData->y_right_bound - m_inputData->y_left_bound);
        particle.vx = std::max(std::min(particle.vx, vx_max), -vx_max);
        particle.vy = std::max(std::min(particle.vy, vy_max), -vy_max);
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