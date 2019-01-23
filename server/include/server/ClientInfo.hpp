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

namespace server {
	class ClientInfo {
	public:
		ClientInfo(connection::Socket socket, player::Player player)
				: _socket{ std::move(socket) }, _player{ std::move(player) } {}
		connection::Socket& get_socket() { return _socket; }
		const connection::Socket& get_socket() const { return _socket; }
		player::Player& get_player() { return _player; }
		const player::Player& get_player() const { return _player; }
	private:
		connection::Socket _socket;
		player::Player _player;
	};
}

