#pragma once
#include <exception>
#include <functional>
#include <string>
#include <vector>

namespace validate {
	using StringArgs = std::vector<std::string>;
	using Validator = std::function<bool(StringArgs)>;

	struct ValidationException : public std::exception
	{
		ValidationException(const char* msg) : std::exception(msg) {}
	};

	template<class T>
	bool validate_that(T arg, std::function<bool(T)> func) {
		return func(arg);
	}

	template<class T>
	bool is_empty(std::vector<T> vector) {
		return vector.empty();
	}

	inline bool is_single_digit(std::string str) {
		try {
			auto digit = std::stoi(str);
			return digit < 10 && digit >= 0;
		}
		catch(...){
			return false;
		}
	}

	inline std::function<bool(std::string)> is_between(int min, int max) {
		return [=](std::string str) {try 
		{
			auto digit = std::stoi(str);
			return digit < max && digit >= min;
		}
		catch (...) {
			return false;
		}};
	}
}
