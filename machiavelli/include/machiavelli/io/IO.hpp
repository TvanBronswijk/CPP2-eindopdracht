#pragma once
#include <vector>
#include <string>

namespace io {
	template<class T> std::vector<T> parse(std::string);
	template<class T> void write(std::string, std::vector<T>);
};
