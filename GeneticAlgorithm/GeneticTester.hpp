#ifndef GENETICALGORITHM_GENETICTESTER_HPP_
#define GENETICALGORITHM_GENETICTESTER_HPP_

#include "GeneticEncoder.hpp"
#include "GeneticBase.hpp"
#include <iostream>
#include <vector>
#include <sstream>

namespace GA {

    class GeneticTester {
    public:
        // Функция для тестирования кодировщика
        static void testGeneticEncoder() {
            std::cout << "\n=== ТЕСТ КОДИРОВЩИКА (ПОЛНЫЙ) ===" << std::endl;

            // Создаем конфигурацию кодировщика
            EncodingConfig config(52, -1000000.0, 1000000.0, -5000000.0, 5000000.0); // 8 бит для наглядности
            GeneticEncoder encoder(config);

            std::cout << "\n--- Тест encodeVariable ---" << std::endl;

            // Тестируем encodeVariable с разными числами
            double testNumber = -599955.32145645;
            std::cout << "Число: " << testNumber << " -> ";

            // Кодируем число в биты
            std::vector<bool> bits = encoder.encodeVariable(testNumber, -900000.0, 900000.0);

            // Выводим биты
            for (bool bit : bits) {
                std::cout << (bit ? "1" : "0");
            }
            std::cout << std::endl;

            std::cout << "\n--- Тест uintToBits ---" << std::endl;

            // Тестируем uintToBits
            uint32_t testUint1 = 59;
            std::cout << "Число " << testUint1 << " -> ";

            std::vector<bool> bits2 = encoder.uintToBits(testUint1, 8);

            for (bool bit : bits2) {
                std::cout << (bit ? "1" : "0");
            }
            std::cout << std::endl;

            std::cout << "\n--- Тест bitsToUint ---" << std::endl;

            // Тестируем bitsToUint - преобразуем биты обратно в число
            std::vector<bool> testBits = { 0, 0, 1, 1, 1, 0, 1, 1 }; // 59 в двоичной
            std::cout << "Биты: ";
            for (bool bit : testBits) {
                std::cout << (bit ? "1" : "0");
            }

            uint32_t result = encoder.bitsToUint(testBits);
            std::cout << " -> Число: " << result << std::endl;
            std::cout << "=======================================" << std::endl;

            // Проверка обратного преобразования
            std::cout << "Проверка (uintToBits → bitsToUint): ";
            uint32_t testUint2 = 59;
            std::cout << "Число " << testUint2 << " -> ";

            auto bits3 = encoder.uintToBits(testUint2, 8);
            for (bool bit : bits3) std::cout << (bit ? "1" : "0");
            uint32_t back = encoder.bitsToUint(bits3);
            std::cout << " -> " << back << " (должно быть: " << testUint2 << ")" << std::endl;

            std::cout << "\n--- Тест decodeVariable ---" << std::endl;

            // Тестируем полный цикл: encodeVariable → decodeVariable
            double originalNumber = 8995436.945435965;
            std::cout << "Исходное число: " << originalNumber << std::endl;

            // Кодируем
            std::vector<bool> encodedBits = encoder.encodeVariable(originalNumber, -9000000.0, 9000000.0);
            std::cout << "Закодированные биты: ";
            for (bool bit : encodedBits) std::cout << (bit ? "1" : "0");
            std::cout << std::endl;

            // Декодируем обратно
            double decodedNumber = encoder.decodeVariable(encodedBits, -9000000.0, 9000000.0);
            std::cout << "Декодированное число: " << decodedNumber << std::endl;

            // Вычисляем ошибку
            double error = std::abs(originalNumber - decodedNumber);
            std::cout << "Ошибка: " << error << std::endl;

            // Тест с граничными значениями
            std::cout << "\n--- Тест граничных значений ---" << std::endl;
            std::vector<double> testNumbers = { -10000.0, 0.0, 10000.0, -500.0, 700.500 };

            for (double num : testNumbers) {
                auto bits = encoder.encodeVariable(num, -1000.0, 1000.0);
                double decoded = encoder.decodeVariable(bits, -1000.0, 1000.0);
                double err = std::abs(num - decoded);
                std::cout << num << " -> " << decoded << " (ошибка: " << err << ")" << std::endl;
            }

            std::cout << "=== ТЕСТ КОДИРОВЩИКА ЗАВЕРШЕН ===\n" << std::endl;
        }

        // Тест кодирования/декодирования точек
        static void testEncodeMethod() {
            std::cout << "=== ТЕСТ МЕТОДА ENCODE ===" << std::endl;

            // Конфигурация: 4 бита на переменную для наглядности
            double x_left_bound = -1000.0;
            double x_right_bound = 1000.0;
            double y_left_bound = -500.0;
            double y_right_bound = 500.0;

            EncodingConfig config(32, x_left_bound, x_right_bound, y_left_bound, y_right_bound);
            GeneticEncoder encoder(config);

            double x = 0.553463243, y = -1.253452243243;
            std::cout << "Исходная точка: (" << x << ", " << y << ")" << std::endl;

            // Кодируем точку
            std::cout << "Исходное число: " << x << std::endl;
            // Кодируем
            std::vector<bool> encodedBits = encoder.encodeVariable(x, x_left_bound, x_right_bound);
            std::cout << "Закодированные биты: ";
            for (bool bit : encodedBits) std::cout << (bit ? "1" : "0");
            std::cout << std::endl;
            std::cout << "Исходное число: " << y << std::endl;
            encodedBits = encoder.encodeVariable(y, y_left_bound, y_right_bound);
            std::cout << "Закодированные биты: ";
            for (bool bit : encodedBits) std::cout << (bit ? "1" : "0");
            std::cout << std::endl;

            std::vector<bool> chromosome = encoder.encode(x, y);
            std::cout << "Хромосома: ";
            for (int i = 0; i < chromosome.size(); i++) {
                std::cout << (chromosome[i] ? "1" : "0");
                if (i == 51) std::cout << " ";
            }
            std::cout << std::endl;

            std::cout << "Длина хромосомы: " << chromosome.size() << " бит" << std::endl;
            std::cout << "Разбивка: " << config.bits_per_variable << " бит X + "
                << config.bits_per_variable << " бит Y" << std::endl;

            //Декодируем обратно для проверки
            Individual decoded = encoder.decode(chromosome);
            std::cout << "Декодированная точка: (" << decoded.x << ", " << decoded.y << ")" << std::endl;
        }

        // Функция для тестирования создания случайных особей
        static void testCreateRandomIndividual() {
            std::cout << "\n=== ТЕСТ СОЗДАНИЯ СЛУЧАЙНЫХ ОСОБЕЙ ===" << std::endl;

            // Создаем конфигурацию кодировщика (используем 16 бит для наглядности)
            EncodingConfig config(52, -10000.0, 10000.0, -5000.0, 5000.0);
            GeneticEncoder encoder(config);

            std::cout << "Диапазон X: [" << config.x_left_bound << ", " << config.x_right_bound << "]" << std::endl;
            std::cout << "Диапазон Y: [" << config.y_left_bound << ", " << config.y_right_bound << "]" << std::endl;
            std::cout << "Бит на переменную: " << config.bits_per_variable << std::endl;
            std::cout << "Всего бит в хромосоме: " << config.total_bits << std::endl;

            // Создаем несколько случайных особей
            std::cout << "\n--- Создание 10 случайных особей с детальным анализом ---" << std::endl;
            for (int i = 0; i < 10; ++i) {
                Individual ind = encoder.createRandomIndividual();

                std::cout << "\n=== ОСОБЬ " << (i + 1) << " ===" << std::endl;
                std::cout << "Сгенерированные координаты: (" << ind.x << ", " << ind.y << ")" << std::endl;

                // Выводим хромосому с разделением на X и Y
                std::cout << "Хромосома (" << ind.chromosome.size() << " бит):" << std::endl;
                std::cout << "  X биты [" << config.bits_per_variable << " бит]: ";
                for (int j = 0; j < config.bits_per_variable; ++j) {
                    std::cout << (ind.chromosome[j] ? "1" : "0");
                }

                std::cout << std::endl << "  Y биты [" << config.bits_per_variable << " бит]: ";
                for (int j = config.bits_per_variable; j < config.total_bits; ++j) {
                    std::cout << (ind.chromosome[j] ? "1" : "0");
                }
                std::cout << std::endl;

                // Декодируем хромосому обратно
                std::cout << "\n--- Декодирование хромосомы ---" << std::endl;
                Individual decoded = encoder.decode(ind.chromosome);
                std::cout << "Декодированные координаты: (" << decoded.x << ", " << decoded.y << ")" << std::endl;

                // Проверяем, что координаты в пределах диапазона
                bool x_in_range = (ind.x >= config.x_left_bound && ind.x <= config.x_right_bound);
                bool y_in_range = (ind.y >= config.y_left_bound && ind.y <= config.y_right_bound);
                std::cout << "Проверка диапазона:" << std::endl;
                std::cout << "  X в диапазоне: " << (x_in_range ? "ДА" : "НЕТ") << std::endl;
                std::cout << "  Y в диапазоне: " << (y_in_range ? "ДА" : "НЕТ") << std::endl;

                // Вычисляем ошибки
                double x_error = std::abs(ind.x - decoded.x);
                double y_error = std::abs(ind.y - decoded.y);
                std::cout << "Точность декодирования:" << std::endl;
                std::cout << "  Ошибка X: " << x_error << std::endl;
                std::cout << "  Ошибка Y: " << y_error << std::endl;

                // Проверяем полный цикл encode/decode
                std::cout << "\n--- Проверка полного цикла ---" << std::endl;
                std::vector<bool> re_encoded = encoder.encode(ind.x, ind.y);
                Individual double_decoded = encoder.decode(re_encoded);
                double full_cycle_x_error = std::abs(ind.x - double_decoded.x);
                double full_cycle_y_error = std::abs(ind.y - double_decoded.y);
                std::cout << "Ошибка полного цикла (generate→encode→decode):" << std::endl;
                std::cout << "  X: " << full_cycle_x_error << std::endl;
                std::cout << "  Y: " << full_cycle_y_error << std::endl;
                std::cout << "\n============================================" << std::endl;
            }

            // Тест на уникальность особей
            std::cout << "\n--- Проверка уникальности 10 особей ---" << std::endl;
            std::vector<Individual> individuals;
            for (int i = 0; i < 10; ++i) {
                individuals.push_back(encoder.createRandomIndividual());
            }

            // Проверяем, что все особи разные
            bool all_unique = true;
            int duplicate_count = 0;
            for (int i = 0; i < individuals.size(); ++i) {
                for (int j = i + 1; j < individuals.size(); ++j) {
                    if (individuals[i].x == individuals[j].x && individuals[i].y == individuals[j].y) {
                        all_unique = false;
                        duplicate_count++;
                        std::cout << "Найдены одинаковые особи " << i << " и " << j
                            << ": (" << individuals[i].x << ", " << individuals[i].y << ")" << std::endl;
                    }
                }
            }

            if (all_unique) {
                std::cout << "✓ Все 10 особей уникальны!" << std::endl;
            }
            else {
                std::cout << "⚠ Найдено " << duplicate_count << " пар одинаковых особей" << std::endl;
            }

            // Дополнительный тест: создание особи с известными координатами
            std::cout << "\n--- Тест с известными координатами ---" << std::endl;
            double test_x = 25.5, test_y = -12.3;
            Individual known_ind(test_x, test_y);
            known_ind.chromosome = encoder.encode(test_x, test_y);

            std::cout << "Исходная точка: (" << test_x << ", " << test_y << ")" << std::endl;
            std::cout << "Хромосома: ";
            for (int j = 0; j < config.bits_per_variable; ++j) {
                std::cout << (known_ind.chromosome[j] ? "1" : "0");
            }
            std::cout << " ";
            for (int j = config.bits_per_variable; j < config.total_bits; ++j) {
                std::cout << (known_ind.chromosome[j] ? "1" : "0");
            }
            std::cout << std::endl;

            Individual decoded_known = encoder.decode(known_ind.chromosome);
            std::cout << "Декодированная точка: (" << decoded_known.x << ", " << decoded_known.y << ")" << std::endl;
            std::cout << "Ошибка: X=" << std::abs(test_x - decoded_known.x)
                << ", Y=" << std::abs(test_y - decoded_known.y) << std::endl;

            std::cout << "\n=== ТЕСТ СОЗДАНИЯ СЛУЧАЙНЫХ ОСОБЕЙ ЗАВЕРШЕН ===\n" << std::endl;
        }

        // Тест инициализации популяции
        static void testInitializePopulation() {
            std::cout << "\n=== ТЕСТ ИНИЦИАЛИЗАЦИИ ПОПУЛЯЦИИ ===" << std::endl;

            // Подготавливаем тестовые данные
            InputData data;
            data.function = "x^2 + y^2";
            data.extremum_type = ExtremumType::MINIMUM;
            data.x_left_bound = -10.0;
            data.x_right_bound = 10.0;
            data.y_left_bound = -10.0;
            data.y_right_bound = 10.0;
            data.result_precision = 8;

            // Конфигурация с маленькой популяцией для теста
            GAConfig config(10, 5, 0.8, 0.05, 16, 2);

            GeneticBase ga;
            Result result = ga.initialize(&data, &config);

            if (result != Result::Success) {
                std::cout << "❌ Ошибка инициализации: " << resultToString(result) << std::endl;
                return;
            }

            // Тестируем инициализацию популяции
            std::cout << "\n--- Проверка до инициализации ---" << std::endl;
            std::cout << "Размер популяции до: " << ga.getPopulation().size() << std::endl;
            std::cout << "Лучший fitness до: " << ga.getBestFitness() << std::endl;

            // Вызываем тестируемый метод
            ga.initializePopulation();

            std::cout << "\n--- Результаты инициализации ---" << std::endl;
            const auto& population = ga.getPopulation();
            std::cout << "Размер популяции после: " << population.size() << std::endl;
            std::cout << "Ожидаемый размер: " << config.population_size << std::endl;

            // Проверяем размер популяции
            if (population.size() == config.population_size) {
                std::cout << "✅ Размер популяции корректен" << std::endl;
            }
            else {
                std::cout << "❌ Ошибка: неверный размер популяции" << std::endl;
            }

            // Проверяем, что все особи в пределах границ
            bool all_in_bounds = true;
            for (size_t i = 0; i < population.size(); ++i) {
                const Individual& ind = population[i];
                bool x_ok = (ind.x >= data.x_left_bound && ind.x <= data.x_right_bound);
                bool y_ok = (ind.y >= data.y_left_bound && ind.y <= data.y_right_bound);

                if (!x_ok || !y_ok) {
                    all_in_bounds = false;
                    std::cout << "❌ Особь " << i << " вне границ: ("
                        << ind.x << ", " << ind.y << ")" << std::endl;
                }
            }

            if (all_in_bounds) {
                std::cout << "✅ Все особи в пределах границ" << std::endl;
            }

            // Проверяем, что fitness вычислен для всех особей
            bool all_have_fitness = true;
            for (size_t i = 0; i < population.size(); ++i) {
                if (population[i].fitness == 0.0 && i > 0) { // Первая особь может иметь fitness = 0
                    all_have_fitness = false;
                    std::cout << "❌ Особь " << i << " не имеет fitness" << std::endl;
                }
            }

            if (all_have_fitness) {
                std::cout << "✅ Fitness вычислен для всех особей" << std::endl;
            }

            // Проверяем сортировку (для минимума первая особь должна иметь наименьший fitness)
            bool correctly_sorted = true;
            for (size_t i = 1; i < population.size(); ++i) {
                if (population[i].fitness < population[0].fitness) {
                    correctly_sorted = false;
                    std::cout << "❌ Популяция не отсортирована правильно" << std::endl;
                    break;
                }
            }

            if (correctly_sorted) {
                std::cout << "✅ Популяция правильно отсортирована" << std::endl;
            }

            // Выводим информацию о лучшей особи
            std::cout << "\n--- Лучшая особь ---" << std::endl;
            std::cout << "Координаты: (" << ga.getBestX() << ", " << ga.getBestY() << ")" << std::endl;
            std::cout << "Fitness: " << ga.getBestFitness() << std::endl;
            std::cout << "Хромосома: ";

            if (!population.empty()) {
                const auto& best_chromosome = population[0].chromosome;
                for (size_t i = 0; i < std::min(best_chromosome.size(), size_t(20)); ++i) {
                    std::cout << (best_chromosome[i] ? "1" : "0");
                }
                if (best_chromosome.size() > 20) {
                    std::cout << "...";
                }
                std::cout << " (" << best_chromosome.size() << " бит)" << std::endl;
            }

            // Выводим статистику по популяции
            std::cout << "\n--- Статистика популяции ---" << std::endl;
            double min_fitness = population[0].fitness;
            double max_fitness = population.back().fitness;
            double avg_fitness = 0.0;

            for (const auto& ind : population) {
                avg_fitness += ind.fitness;
            }
            avg_fitness /= population.size();

            std::cout << "Min fitness: " << min_fitness << std::endl;
            std::cout << "Max fitness: " << max_fitness << std::endl;
            std::cout << "Avg fitness: " << avg_fitness << std::endl;

            std::cout << "=== ТЕСТ ИНИЦИАЛИЗАЦИИ ПОПУЛЯЦИИ ЗАВЕРШЕН ===\n" << std::endl;
        }
    };
} // namespace GA

#endif // GENETICALGORITHM_GENETICTESTER_HPP_