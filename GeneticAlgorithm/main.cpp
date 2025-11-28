//
// Created on 24 Nov, 2025
// by alecproj
//
#include <GeneticAlgorithm/GeneticAlgorithm.hpp>
#include <GeneticAlgorithm/GeneticEncoder.hpp>
#include <glog/logging.h>
#include <muParser.h>
#include <iostream>
#include <string>
#include <variant>

using namespace GA;

// Функция для тестирования кодировщика
void testGeneticEncoder() {
    std::cout << "\n=== ТЕСТ КОДИРОВЩИКА (ПОЛНЫЙ) ===" << std::endl;

    // Создаем конфигурацию кодировщика
    EncodingConfig config(32, -10.0, 10.0, -5.0, 5.0); // 8 бит для наглядности
    GeneticEncoder encoder(config);

    std::cout << "\n--- Тест encodeVariable ---" << std::endl;

    // Тестируем encodeVariable с разными числами
    double testNumber = -5.32145;
    std::cout << "Число: " << testNumber << " -> ";

    // Кодируем число в биты
    std::vector<bool> bits = encoder.encodeVariable(testNumber, -10.0, 10.0);

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
    std::cout << " -> " << back << " (должно быть: "<< testUint2 << ")" << std::endl;

    std::cout << "\n--- Тест decodeVariable ---" << std::endl;

    // Тестируем полный цикл: encodeVariable → decodeVariable
    double originalNumber = 7.9454359;
    std::cout << "Исходное число: " << originalNumber << std::endl;

    // Кодируем
    std::vector<bool> encodedBits = encoder.encodeVariable(originalNumber, -10.0, 10.0);
    std::cout << "Закодированные биты: ";
    for (bool bit : encodedBits) std::cout << (bit ? "1" : "0");
    std::cout << std::endl;

    // Декодируем обратно
    double decodedNumber = encoder.decodeVariable(encodedBits, -10.0, 10.0);
    std::cout << "Декодированное число: " << decodedNumber << std::endl;

    // Вычисляем ошибку
    double error = std::abs(originalNumber - decodedNumber);
    std::cout << "Ошибка: " << error << std::endl;

    // Тест с граничными значениями
    std::cout << "\n--- Тест граничных значений ---" << std::endl;
    std::vector<double> testNumbers = { -10.0, 0.0, 10.0, -5.0, 7.5 };

    for (double num : testNumbers) {
        auto bits = encoder.encodeVariable(num, -10.0, 10.0);
        double decoded = encoder.decodeVariable(bits, -10.0, 10.0);
        double err = std::abs(num - decoded);
        std::cout << num << " -> " << decoded << " (ошибка: " << err << ")" << std::endl;
    }

    std::cout << "=== ТЕСТ КОДИРОВЩИКА ЗАВЕРШЕН ===\n" << std::endl;
}


// Класс-заглушка - реализовывать не нужно!
class MockReporter {
    using Cell = std::variant<std::string, double, long long, bool>;
public:
    int begin() { return 0; }
    int end() { return 0; }
    void insertValue(const std::string &name, double value) {
        LOG(WARNING) << name << ": " << value;
    }
    void insertMessage(const std::string &text) {
        LOG(WARNING) << text;
    }
    int beginTable(const std::string &title, const std::vector<std::string> &columnHeaders) {
        static int tid = 0;
        LOG(WARNING) << "Открыта таблица: " << title;
        std::ostringstream stream;
        for (const auto h : columnHeaders) {
            stream << h << " ";
        }
        LOG(WARNING) << stream.str();
        return ++tid;
    }
    int insertRow(int tableId, const std::vector<Cell> &row) {
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

int main(int argc, char *argv[])
{
    FLAGS_logtostderr = true;
    FLAGS_alsologtostderr = false;
    FLAGS_stderrthreshold = 0;
    FLAGS_log_prefix = false;
    FLAGS_colorlogtostderr = true;
    FLAGS_minloglevel = 0;
    google::InitGoogleLogging(argv[0]);
    google::SetVLOGLevel("*", DEBUG);

    testGeneticEncoder();
    //LOGERR(Result::ComputeError);
    
    //LOG(WARNING) << "\nПредупреждение!";
    //LOG(ERROR) << "Ошибка!";

    //LOG(DEBUG) << "Дебаг!";
    //LOG(INFO) << "Информация!";

    using AlgoType = GeneticAlgorithm<MockReporter>;
    MockReporter reporter{};
    AlgoType algo{ &reporter };
    InputData data{};

    // Ввод данных от пользователя
    std::cout << "Введите функцию (например, x^2 + y^2): ";
    std::getline(std::cin, data.function);
    std::string input_str;

    try {
        std::cout << "Введите тип экстремума (MINIMUM или MAXIMUM): ";
        std::cin >> input_str;
        data.extremum_type = stringToExtremumType(input_str);

        // НАСТРОЙКИ ГРАДИЕНТНЫХ МЕТОДОВ
        std::cout << "=== НАСТРОЙКИ ГРАДИЕНТНОГО МЕТОДА ===" << std::endl;

        // ОБЩИЕ ПАРАМЕТРЫ ДЛЯ ВСЕХ АЛГОРИТМОВ
        std::cout << "=== ОБЩИЕ ПАРАМЕТРЫ ===" << std::endl;
        std::cout << "Введите левую границу X: ";
        std::cin >> data.x_left_bound;
        std::cout << "Введите правую границу X: ";
        std::cin >> data.x_right_bound;
        std::cout << "Введите левую границу Y: ";
        std::cin >> data.y_left_bound;
        std::cout << "Введите правую границу Y: ";
        std::cin >> data.y_right_bound;
        std::cout << "Введите точность результата (например, 8): ";
        std::cin >> data.result_precision;
        std::cout << "Введите точность вычислений (например, 15): ";
        std::cin >> data.computation_precision;
    }
    catch (const std::exception& e) {
        std::cout << "Ошибка ввода: " << e.what() << std::endl;
        return 1;
    }

    // Проверка и установка данных (вызовет ошибки из GradientDescent, если неверно)
    auto rv = algo.setInputData(&data);
    if (rv != Result::Success) {
        std::cout << "Ошибка установки данных: " << resultToString(rv) << std::endl;
        return 1;
    }

    // Запуск алгоритма
    rv = algo.solve();

    // Вывод результатов
    std::cout << std::endl << "=== РЕЗУЛЬТАТЫ ===" << std::endl;
    // std::cout << "Оптимум в точке: (" << algo.getX() << ", " << algo.getY() << ")" << std::endl;
    // std::cout << "Значение функции: " << algo.getOptimumValue() << std::endl;
    // std::cout << "Итераций: " << algo.getIterations() << ", Вызовов функции: " << algo.getFunctionCalls() << std::endl;

    google::ShutdownGoogleLogging();
    return 0;
}
