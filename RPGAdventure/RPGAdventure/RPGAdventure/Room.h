#pragma once

#include <vector>

#include "flecs/flecs.h"
#include "raylib/raylib.h"

class Room {

public:
	std::vector<flecs::entity> entitiesInThisRoom;
	Vector2 roomDimensions = { 0 };
};