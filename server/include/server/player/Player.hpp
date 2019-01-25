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
#include "PlayerData.hpp"
#include "state/StateMachine.hpp"

namespace server::player {
	class Player {
	public:
		Player() = default;
		Player(const std::string name, std::unique_ptr<PlayerData> data) : _name{ name }, _data{ std::move(data) } {}
		Player(const std::string name, std::unique_ptr<PlayerData> data, std::unique_ptr<state::State> initial_state)
		: _name{ name }, _data{ std::move(data) }, _state{ std::move(initial_state) } {}

		std::string get_name() const { return _name; }
		template<class T>
		T& get_data() {
			static_assert(std::is_convertible<T*, PlayerData*>::value, "T must be a concrete implementation of PlayerData");
			return *(static_cast<T*>(_data.get()));
		}
		template<class T>
		const T& get_data() const {
			static_assert(std::is_convertible<T*, PlayerData*>::value, "T must be a concrete implementation of PlayerData");
			return *(static_cast<T*>(_data.get()));
		}
		state::StateMachine& get_states() { return _state; }
		const state::StateMachine& get_states() const {return _state; }
	private:
		std::string _name;
		std::unique_ptr<PlayerData> _data;
		state::StateMachine _state;
	};
}
