#pragma once
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>

namespace io {
	template<class T>
	std::vector<T> parse(std::string file_location) {
		std::vector<T> result{};

		std::ifstream file{};
		try {
            file.open(file_location);
            T card{};
            while (file >> card) {
                result.push_back(std::move(card));
            }
            file.close();
		}catch(...){
		    std::cerr << "Something went wrong during file parsing.";
		}
		return result;
	}

	template<class T>
	void write(std::string file_location, std::vector<T> to_write) {
		std::ofstream file{};
		file.open(file_location);
		std::for_each(to_write.begin(), to_write.end(), [&](T buildingcard) { file << buildingcard; });
		file.close();
	}
};
