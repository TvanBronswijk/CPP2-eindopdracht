#pragma once
#include <random>
#include <ctime>

namespace util {
	class Random {
	public:
		Random() : Random(std::random_device()()) {}
		explicit Random(unsigned long seed_value) { _generator.seed(seed_value); }
		template<class T> T next() { return next(0, 1); }
		template<class T> T next(T max) { return next(0, max); }
		template<class T> T next(T min, T max) { return std::uniform_int_distribution<T>(min, max)(_generator); }
	private:
		std::default_random_engine _generator;
	};
}
