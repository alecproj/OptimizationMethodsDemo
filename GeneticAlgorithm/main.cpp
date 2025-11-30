//
// Created on 24 Nov, 2025
// by alecproj
//
#include <GeneticAlgorithm/GeneticAlgorithm.hpp>
#include <GeneticAlgorithm/GeneticEncoder.hpp>
#include <GeneticAlgorithm/GeneticTester.hpp>
#include <glog/logging.h>
#include <muParser.h>
#include <iostream>
#include <string>
#include <variant>
#include <limits>

using namespace GA;

// Класс-заглушка - реализовывать не нужно!
class MockReporter {
    using Cell = std::variant<std::string, double, long long, bool>;
public:
    int begin() { return 0; }
    int end() { return 0; }
    void insertValue(const std::string& name, double value) {
        LOG(WARNING) << name << ": " << value;
    }
    void insertMessage(const std::string& text) {
        LOG(WARNING) << text;
    }
    int beginTable(const std::string& title, const std::vector<std::string>& columnHeaders) {
        static int tid = 0;
        LOG(WARNING) << "Открыта таблица: " << title;
        std::ostringstream stream;
        for (const auto h : columnHeaders) {
            stream << h << " ";
        }
        LOG(WARNING) << stream.str();
        return ++tid;
    }
    int insertRow(int tableId, const std::vector<Cell>& row) {
        std::ostringstream stream;
        for (const auto v : row) {
            std::visit([&stream](auto&& value) {
                stream << value;
                }, v);
            stream << " ";
        }
        LOG(WARNING) << tableId << ": " << stream.str();
        return 0;
    }
    void insertResult(double x, double y, double funcValue) {
        LOG(WARNING) << "Ответ: x=" << x << " y=" << y << " f=" << funcValue;
    }
    void endTable(int tableId) {
        LOG(WARNING) << "Таблица " << tableId << " закрыта";
    }
};

// Конвертация типа экстремума в строку
inline std::string extremumTypeToString(ExtremumType type) {
    switch (type) {
    case ExtremumType::MINIMUM: return "Минимум";
    case ExtremumType::MAXIMUM: return "Максимум";
    default:                    return "";
    }
}

ExtremumType stringToExtremumType(const std::string& str) {
    if (str == "MINIMUM") return ExtremumType::MINIMUM;
    if (str == "MAXIMUM") return ExtremumType::MAXIMUM;
    throw std::invalid_argument("Неверный тип экстремума");
}

// Функция для безопасного ввода чисел
template<typename T>
T safeInput(const std::string& prompt, T min_val = std::numeric_limits<T>::lowest(),
    T max_val = std::numeric_limits<T>::max()) {
    T value;
    while (true) {
        std::cout << prompt;
        std::cin >> value;
        if (std::cin.fail() || value < min_val || value > max_val) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Ошибка! Введите корректное значение от " << min_val
                << " до " << max_val << ": ";
        }
        else {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }
    }
}

int main(int argc, char* argv[])
{
    FLAGS_logtostderr = true;
    FLAGS_alsologtostderr = false;
    FLAGS_stderrthreshold = 0;
    FLAGS_log_prefix = false;
    FLAGS_colorlogtostderr = true;
    FLAGS_minloglevel = 0;
    google::InitGoogleLogging(argv[0]);
    google::SetVLOGLevel("*", DEBUG);

    // === ТЕСТИРОВАНИЕ ===
    // Раскомментируйте нужные тесты:
    // GeneticTester::testGeneticEncoder();
    // GeneticTester::testEncodeMethod();
    // GeneticTester::testCreateRandomIndividual();
    // GeneticTester::testInitializePopulation();

    using AlgoType = GeneticAlgorithm<MockReporter>;
    MockReporter reporter{};
    AlgoType algo{ &reporter };
    InputData data{};
    GAConfig config{};

    std::cout << "=== ГЕНЕТИЧЕСКИЙ АЛГОРИТМ ДЛЯ ОПТИМИЗАЦИИ ФУНКЦИЙ ===" << std::endl;
    std::cout << "=== ВВОД ПАРАМЕТРОВ ФУНКЦИИ И ОБЛАСТИ ПОИСКА ===" << std::endl;

    // Ввод данных функции
    std::cout << "Введите функцию (например, x^2 + y^2): ";
    std::getline(std::cin, data.function);

    std::string input_str;
    std::cout << "Введите тип экстремума (MINIMUM или MAXIMUM): ";
    std::cin >> input_str;
    data.extremum_type = stringToExtremumType(input_str);
    std::cin.ignore();

    // Ввод границ поиска
    std::cout << "\n=== ГРАНИЦЫ ПОИСКА ===" << std::endl;
    data.x_left_bound = safeInput<double>("Введите левую границу X: ");
    data.x_right_bound = safeInput<double>("Введите правую границу X: ", data.x_left_bound + 1e-10);

    data.y_left_bound = safeInput<double>("Введите левую границу Y: ");
    data.y_right_bound = safeInput<double>("Введите правую границу Y: ", data.y_left_bound + 1e-10);

    // Ввод точности
    std::cout << "\n=== ПАРАМЕТРЫ ТОЧНОСТИ ===" << std::endl;
    data.result_precision = safeInput<int>("Введите точность результата (1-15): ", 1, 15);
    data.computation_precision = safeInput<int>("Введите точность вычислений (1-15): ", 1, 15);

    // Ввод параметров генетического алгоритма
    std::cout << "\n=== ПАРАМЕТРЫ ГЕНЕТИЧЕСКОГО АЛГОРИТМА ===" << std::endl;
    config.population_size = safeInput<size_t>("Размер популяции (рекомендуется 50-200): ", 10, 1000);
    config.generations = safeInput<size_t>("Максимальное количество поколений (рекомендуется 100-1000): ", 10, 10000);
    config.crossover_rate = safeInput<double>("Вероятность кроссовера (0.0-1.0, рекомендуется 0.7-0.9): ", 0.0, 1.0);
    config.mutation_rate = safeInput<double>("Вероятность мутации (0.0-0.1, рекомендуется 0.01-0.05): ", 0.0, 0.1);
    config.bits_per_variable = safeInput<uint64_t>("Бит на переменную (1-52, рекомендуется 16-32): ", 1, 52);
    config.elite_count = safeInput<size_t>("Количество элитных особей (рекомендуется 1-5): ", 0, 20);
    data.count_function_calls = safeInput<size_t>("Количество вызовов функции (до 100000): ", 1, 100000);

    // Вывод введенных параметров для подтверждения
    std::cout << "\n=== ПОДТВЕРЖДЕНИЕ ПАРАМЕТРОВ ===" << std::endl;
    std::cout << "Функция: " << data.function << std::endl;
    std::cout << "Тип экстремума: " << extremumTypeToString(data.extremum_type) << std::endl;
    std::cout << "Область поиска: X[" << data.x_left_bound << ", " << data.x_right_bound
        << "], Y[" << data.y_left_bound << ", " << data.y_right_bound << "]" << std::endl;
    std::cout << "Точность: результат=" << data.result_precision
        << ", вычисления=" << data.computation_precision << std::endl;
    std::cout << "Параметры ГА: популяция=" << config.population_size
        << ", поколения=" << config.generations
        << ", кроссовер=" << config.crossover_rate
        << ", мутация=" << config.mutation_rate
        << ", бит=" << config.bits_per_variable
        << ", элита=" << config.elite_count << std::endl;

    std::cout << "\nЗапускаем алгоритм..." << std::endl;

    try {
        // Проверка и установка данных
        auto rv = algo.setInputData(&data, &config);
        if (rv != Result::Success) {
            std::cout << "Ошибка установки данных: " << resultToString(rv) << std::endl;
            return 1;
        }

        // Запуск алгоритма
        rv = algo.solve();

        // Вывод результатов
        std::cout << "\n=== РЕЗУЛЬТАТЫ ===" << std::endl;
        std::cout << "Оптимум в точке: (" << algo.getBestX() << ", " << algo.getBestY() << ")" << std::endl;
        std::cout << "Значение функции: " << algo.getBestFitness() << std::endl;
        std::cout << "Поколений: " << algo.getIteraionts() << ", Вызовов функции: " << algo.getFunctionCalls() << std::endl;

        if (rv == Result::Success) {
            std::cout << "✅ Алгоритм успешно завершил работу!" << std::endl;
        }
        else {
            std::cout << "⚠ Алгоритм завершил работу с кодом: " << resultToString(rv) << std::endl;
        }

    }
    catch (const std::exception& e) {
        std::cout << "❌ Произошла ошибка: " << e.what() << std::endl;
        return 1;
    }

    google::ShutdownGoogleLogging();
    return 0;
}