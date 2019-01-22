#pragma once
//
// ClientInfo.h
//
// Created by Jeroen de Haas on 22/11/2016.
// Copyright (c) 2016 Avans Hogeschool, 's-Hertogenbosch. All rights reserved.
//

#include <string>
#include <utility>

#include "connection/Socket.hpp"
#include "player/Player.hpp"
#include "state/StateMachine.h"

class ClientInfo {
	Socket _socket;
	Player _player;
	StateMachine _states;
public:
public:
	ClientInfo(Socket socket, Player player)
			: _socket{ std::move(socket) }, _player{ std::move(player) } {}
	ClientInfo(Socket socket, Player player, std::unique_ptr<State> initial_state)
		: _socket{ std::move(socket) }, _player{ std::move(player) }, _states{ std::move(initial_state) } {}
	Socket& get_socket() { return _socket; }
	const Socket& get_socket() const { return _socket; }
	Player& get_player() { return _player; }
	const Player& get_player() const { return _player; }
	StateMachine& get_states() { return _states; }
	const StateMachine& get_states() const { return _states; }
};
