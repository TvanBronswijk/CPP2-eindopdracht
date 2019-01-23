#pragma once

#include <exception>
#include <functional>
#include <string>
#include <vector>

namespace server::command::validate {
    using StringArgs = std::vector<std::string>;
    using Validator = std::function<bool(StringArgs)>;

    struct ValidationException : public std::exception {
        const char *msg;

        explicit ValidationException(const char *msg) : std::exception(), msg(msg) {}
    };

    template<class T>
    bool validate_that(T arg, std::function<bool(T)> func) {
        return func(arg);
    }

    template<class T>
    bool is_empty(std::vector<T> vector) {
        return vector.empty();
    }

    template<class T>
    std::function<bool(std::vector<T>)> has_arg(size_t index) {
        return [=](std::vector<T> vector) {
            return vector.size() > index;
        };
    }

    template<class T>
    bool is_single_digit(T str) {
        try {
            auto digit = std::stoi(str);
            return digit < 10 && digit >= 0;
        }
        catch (...) {
            return false;
        }
    }

    template<class T>
    std::function<bool(T)> is_between(int min, int max) {
        return [=](T str) {
            try {
                auto digit = std::stoi(str);
                return digit < max && digit >= min;
            }
            catch (...) {
                return false;
            }
        };
    }
}
