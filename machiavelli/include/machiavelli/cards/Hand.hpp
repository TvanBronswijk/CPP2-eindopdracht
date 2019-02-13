#pragma once
#include <initializer_list>
#include <vector>
#include "util.hpp"

template<class T>
class Hand {
public:
    Hand() = default;
    Hand(std::initializer_list<T> cards) : _cards(cards) {}
    Hand(std::vector<T> cards) : _cards(cards) {}
    void add(const T& t) { _cards.push_back(t); }
    void add(T&& t) { _cards.push_back(t); }
    void remove(size_t i) { remove(_cards.at(i)); }
    void remove(T& t) { _cards.erase(std::remove(_cards.begin(), _cards.end(), t), _cards.end()); }
    T take(size_t i) {
        T result(_cards.at(i));
        remove(i);
        return result;
    }
    T take(T& t){
        T result(t);
        remove(t);
        return result;
    }
    T take_random() { return take(util::RAND.next(0, static_cast<int>(_cards.size()-1))); }

    auto begin() { return _cards.begin(); }
    auto end() { return _cards.end(); }
    auto size() { return _cards.size(); }
private:
    std::vector<T> _cards;
};
