#pragma once

#include <vector>

#include "raylib/raymath.h"
#include "SpriteSheet.h"

class Tile {

public:

	Rectangle tileInSpriteSheet = { 0.0f, 0.0f, 0.0f, 0.0f };
};

class TileMap {

public:

	SpriteSheet tilePallet;
	std::vector<std::vector<Tile>> tiles;

	unsigned int sizeX;
	unsigned int sizeY;
};