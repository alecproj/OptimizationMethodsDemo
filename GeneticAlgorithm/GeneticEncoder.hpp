#ifndef GENETICALGORITHM_GENETICENCODER_HPP_
#define GENETICALGORITHM_GENETICENCODER_HPP_

#include "Common.hpp"
#include <vector>
#include <random>
#include <cstdint>
#include <cmath>

namespace GA {

	// Конфигурация кодирования
	struct EncodingConfig {
		uint64_t bits_per_variable;			// Количество бит на переменную
		uint64_t total_bits;				// Количество бит в хромосоме (2 * bits_per_variable)
		double x_left_bound, x_right_bound; // Границы для X
		double y_left_bound, y_right_bound;	// Границы для Y

		// Инициализация конструктора по умолчанию
		EncodingConfig(uint64_t bits = 32,
					   double x_min = -10.0, double x_max = 10.0,
					   double y_min = -10.0, double y_max = 10.0)
			: bits_per_variable(bits),
			  total_bits(bits * 2),
			  x_left_bound(x_min),
			  x_right_bound(x_max),
			  y_left_bound(y_min),
			  y_right_bound(y_max) {}
	};

	class GeneticEncoder {
	private:
		EncodingConfig m_config;
		mutable std::mt19937 m_rnd;

		
		static constexpr uint64_t MAX_SAFE_BITS = 52; // Максимальное количество бит

	public:
		GeneticEncoder(const EncodingConfig& config = EncodingConfig());

		// Основные методы кодирования/декодирования
		
		// Кодирование
		std::vector<bool> encode(double x, double y) const;
		// Декодирование
		Individual decode(const std::vector<bool>& chromosome) const;
		// Создание случайной особи
		Individual createRandomIndividual() const;

		// Кодирование координаты
		std::vector<bool> encodeVariable(double value, double min_val, double max_val) const;
		// Декодирование координаты
		double decodeVariable(const std::vector<bool>& bits, double min_val, double max_val) const;
		// Преобразование вектора битов в целое число
		uint64_t bitsToUint(const std::vector<bool>& bits) const;
		// Преобразование целого числа в вектор битов фиксированной длины
		std::vector<bool> uintToBits(uint64_t value, uint64_t num_bits) const;

		// Геттеры
		const EncodingConfig& getConfig() const { return m_config; }
		uint64_t getChromosomeLength() const { return m_config.total_bits; }


	};

} // namespace GA

#endif // GENETICALGORITHM_GENETICENCODER_HPP_