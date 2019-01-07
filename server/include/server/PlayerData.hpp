#pragma once

struct PlayerData {
	PlayerData() = default;
	virtual ~PlayerData() = default;
	PlayerData(const PlayerData&) = delete;
	PlayerData(PlayerData&&) = delete;
	PlayerData& operator=(const PlayerData&) = delete;
	PlayerData& operator=(PlayerData&&) = delete;
};

