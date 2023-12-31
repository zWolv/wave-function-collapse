#pragma once
#ifndef TILE_H
#define TILE_H

#include "TileType.h"
#include <iostream>
#include <vector>

class Tile {
public:
	Tile();
	Tile(TileType input);
	TileType type;
	inline static const std::vector<TileType> ALL = { TileType::BEACH, TileType::GRASS, TileType::WATER };
	bool operator==(const Tile& other) const;
	bool operator<(const Tile& other) const;
	friend std::ostream& operator<<(std::ostream& os, const Tile& tile);
	friend Tile operator&(const Tile& lhs, const Tile& rhs);
};



#endif
