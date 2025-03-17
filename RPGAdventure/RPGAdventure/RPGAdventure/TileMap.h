#pragma once

#include <vector>

#include "raylib/raymath.h"
#include "SpriteSheet.h"

typedef struct float2 {
	float v[2];
} float2;

class Tile {

public:

	Rectangle tileInSpriteSheet = { 0.0f, 0.0f, 0.0f, 0.0f };
};

class TileMap {

public:

	SpriteSheet tilePallet;

	std::vector<float2> tileTextureIndexData;
};