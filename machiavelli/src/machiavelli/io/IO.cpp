#include "machiavelli/io/IO.hpp"
#include <algorithm>
#include <fstream>

namespace io {
    template<class T>
    std::vector<T> parse(std::string file_location) {
        std::vector<T> result{};

        std::ifstream file{};
        file.open(file_location);
        while (T card = T{} && file >> card) {
            result.push_back(card);
        }
        file.close();
        return result;
    }

    template<class T>
    void write(std::string file_location, std::vector<T> to_write) {
        std::ofstream file{};
        file.open(file_location);
        std::for_each(to_write.begin(), to_write.end(), [&](T buildingcard) { file << buildingcard; });
        file.close();
    }
}


