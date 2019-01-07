#pragma once
//
//  Player.hpp
//  socketexample
//
//  Created by Bob Polis on 23-11-15.
//  Copyright © 2015 Avans Hogeschool, 's-Hertogenbosch. All rights reserved.
//

#include <string>
#include <memory>
#include "server/PlayerData.hpp"

class Player {
public:
	Player() {}
	Player(const std::string name) : name{ name } {}
	Player(const std::string name, std::unique_ptr<PlayerData> data) : name{ name }, _data{ std::move(data) } {}

	std::string get_name() const { return name; }
	void set_name(const std::string new_name) { name = new_name; }

	template<class T>
	T& get_data() {
		static_assert(std::is_convertible<T*, PlayerData*>::value, "T must be a concrete implementation of PlayerData");
		return *(static_cast<T*>(_data));
	}
private:
	std::string name;
	std::unique_ptr<PlayerData> _data;
};
