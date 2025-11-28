#include "GeneticEncoder.hpp"
#include <glog/logging.h>
#include <stdexcept>
#include <limits>

namespace GA {

	// Конструктор по умолчанию
	GeneticEncoder::GeneticEncoder(const EncodingConfig& config)
		: m_config(config), m_rnd(std::random_device{}()) {}

	// Кодирование координаты
	std::vector<bool> GeneticEncoder::encodeVariable(double value, double min_val, double max_val) const
	{
		// Ограничиваем значение в диапазоне
		value = std::max(min_val, std::min(value, max_val));

		// Нормализуем в [0, 1]
		double normalized = (value - min_val) / (max_val - min_val);

		// Преобразуем в целое число
		uint32_t max_int = (1ULL << m_config.bits_per_variable) - 1;
		uint32_t int_value = static_cast<uint32_t>(normalized * max_int);

		// Преобразуем в биты
		return uintToBits(int_value, m_config.bits_per_variable);
	}

	// Преобразование целого десятичного числа в двоичное представление числа
	std::vector<bool> GeneticEncoder::uintToBits(uint32_t value, uint32_t num_bits) const
	{
		std::vector<bool> bits(num_bits);
		for (uint32_t i = 0; i < num_bits; ++i) {
			uint32_t bit_pos = num_bits - 1 - i;
			bits[bit_pos] = (value >> i) & 1;
		}
		return bits;
	}

	// Декодирование координаты
	double GeneticEncoder::decodeVariable(const std::vector<bool>& bits, double min_val, double max_val) const
	{
		if (bits.size() != m_config.bits_per_variable) {
			throw std::invalid_argument("Неверная длина вектора битов");
		}

		// Преобразуем биты в целое число
		uint32_t int_value = bitsToUint(bits);

		// Нормализуем в [0, 1]
		uint32_t max_int = (1ULL << m_config.bits_per_variable) - 1;
		double normalized = static_cast<double>(int_value) / max_int;

		// Преобразуем в исходную координату
		return min_val + normalized * (max_val - min_val);

	}

	// Преобразование двоичного представления числа в целое десятичное число
	uint32_t GeneticEncoder::bitsToUint(const std::vector<bool>& bits) const
	{
		uint32_t result = 0;
		for (bool bit : bits) {
			result = (result << 1) | (bit ? 1 : 0);
		}
		return result;
	}

	// Кодирование числа в код
	/*std::vector<bool> GeneticEncoder::encode(double x, double y) const
	{
		std::vector<bool> chromosome;
		chromosome.reserve(m_config.total_bits);

		// Кодируем X
		
	}*/




} // namespace GA