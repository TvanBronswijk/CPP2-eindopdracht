#pragma once
#include <random>
#include <ctime>

inline int random_int(int min, int max) {
	std::default_random_engine generator;
	generator.seed(time(0));

	std::uniform_int_distribution<int> distribution1(min, max);

	return distribution1(generator);
}
