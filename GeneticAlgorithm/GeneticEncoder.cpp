#include "GeneticEncoder.hpp"
#include <glog/logging.h>
#include <stdexcept>
#include <limits>

namespace GA {

	// Конструктор по умолчанию
	GeneticEncoder::GeneticEncoder(const EncodingConfig& config)
		: m_config(config), m_rnd(std::random_device{}()) 
	{
		// Проверка переполнения
		if (m_config.bits_per_variable > MAX_SAFE_BITS) {
			throw std::invalid_argument(
				"Превышено максимальное безопасное количество бит: " +
				std::to_string(MAX_SAFE_BITS) +
				" (ограничение мантиссы типа double)");
		}

	}

	// Кодирование координаты
	std::vector<bool> GeneticEncoder::encodeVariable(double value, double min_val, double max_val) const
	{
		// Проверка корректности диапазона
		if (min_val >= max_val) {
			throw std::invalid_argument("Левая граница диапазона должна быть меньше правой границы диапазона");
		}

		// Ограничиваем значение в диапазоне
		value = std::max(min_val, std::min(value, max_val));

		// Нормализуем в [0, 1]
		double normalized = (value - min_val) / (max_val - min_val);

		// Вычисление максимального целого значения от количества бит
		uint64_t max_int = (1ULL << m_config.bits_per_variable) - 1;
		uint64_t int_value = static_cast<uint64_t>(normalized * max_int);

		if (int_value > max_int) {
			int_value = max_int;
		}
		// Преобразуем в биты
		return uintToBits(int_value, m_config.bits_per_variable);
	}

	// Преобразование целого десятичного числа в двоичное представление числа
	std::vector<bool> GeneticEncoder::uintToBits(uint64_t value, uint64_t num_bits) const
	{
		std::vector<bool> bits(num_bits);
		for (uint64_t i = 0; i < num_bits; ++i) {
			uint64_t bit_pos = num_bits - 1 - i;
			bits[bit_pos] = (value >> i) & 1;
		}
		return bits;
	}

	// Декодирование координаты
	double GeneticEncoder::decodeVariable(const std::vector<bool>& bits, double min_val, double max_val) const
	{
		// Проверка равенства длин векторов
		if (bits.size() != m_config.bits_per_variable) {
			throw std::invalid_argument(
				"Неверная длина вектора битов. Ожидалось: " +
				std::to_string(m_config.bits_per_variable) +
				", а было получено: " + std::to_string(bits.size()));
		}

		// Проверка корректности диапазона
		if (min_val >= max_val) {
			throw std::invalid_argument("Левая граница диапазона должна быть меньше правой границы диапазона");
		}

		// Преобразуем биты в целое число
		uint64_t int_value = bitsToUint(bits);

		// Вычисление максимального целого значения от количества бит
		uint64_t max_int = (1ULL << m_config.bits_per_variable) - 1;

		// Нормализуем в [0, 1]
		double normalized = static_cast<double>(int_value) / max_int;

		// Преобразуем в исходную координату
		return min_val + normalized * (max_val - min_val);
	}

	// Преобразование двоичного представления числа в целое десятичное число
	uint64_t GeneticEncoder::bitsToUint(const std::vector<bool>& bits) const
	{
		uint64_t result = 0;
		for (bool bit : bits) {
			result = (result << 1) | (bit ? 1 : 0);
		}
		return result;
	}

	// Кодирование точки (x, y) в код
	std::vector<bool> GeneticEncoder::encode(double x, double y) const
	{
		std::vector<bool> chromosome;
		// Выделение памяти, почему так называется
		chromosome.reserve(m_config.total_bits);

		// Кодируем X
		auto x_bits = encodeVariable(x, m_config.x_left_bound, m_config.x_right_bound);
		chromosome.insert(chromosome.end(), x_bits.begin(), x_bits.end());

		// Кодируем Y
		auto y_bits = encodeVariable(y, m_config.y_left_bound, m_config.y_right_bound);
		chromosome.insert(chromosome.end(), y_bits.begin(), y_bits.end());

		return chromosome;
	}


} // namespace GA