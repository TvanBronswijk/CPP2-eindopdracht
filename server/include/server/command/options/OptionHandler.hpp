#pragma once
#include <vector>

template<class T>
class OptionHandler {
public:	
	OptionHandler() = default;
	OptionHandler(std::vector<T*> vect, std::function<void(T&)> function) : options(vect), func_(function) {}
	void choose(size_t choice) {
		auto option = options.at(choice);
		if (option)
			func_(*option);
		throw; //TODO throw exception
	}
private:
	std::function<void(T&)> func_;
	std::vector<T*> options;
};
