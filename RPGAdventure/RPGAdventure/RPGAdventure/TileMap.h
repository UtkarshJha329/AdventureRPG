#pragma once

#include <vector>

#include "raylib/raymath.h"
#include "SpriteSheet.h"

#include "Room.h"

typedef struct float2 {
	float v[2];
} float2;

std::vector<float> blockingTilesX = { 4, 9 };
std::vector<Vector2> blockingTileIndices = { Vector2{8.0, 0.0} };

class Tile {

public:

	Rectangle tileInSpriteSheet = { 0.0f, 0.0f, 0.0f, 0.0f };
};

class TileMap {

public:

	SpriteSheet tilePallet;

	std::vector<float2> tileTextureIndexData;

	std::vector<std::vector<Room>> roomsData;
};

void SpawnRooms(TileMap& tm) {

    for (int y = 0; y < totalRoomsY; y++)
    {
        tm.roomsData.push_back(std::vector<Room>(totalRoomsX));
        for (int x = 0; x < totalRoomsX; x++)
        {
            Room curRoom;
            curRoom.indexX = x;
            curRoom.indexY = y;

            tm.roomsData[y][x] = curRoom;
        }
    }
}

bool IsTileFilledWithCollider(TileMap& tm, Vector2 tileToCheckIndex, int numTilesX, bool roomMode = false) {

    if (roomMode) {
        bool borderTileX = (int)tileToCheckIndex.x % (int)(11) == 0 || (int)tileToCheckIndex.x % (int)(12) == 0;
        bool borderTileY = (int)tileToCheckIndex.y % (int)(6) == 0 || (int)tileToCheckIndex.y % (int)(7) == 0;

        if (borderTileX || borderTileY) {
            return true;
        }
    }

    for (int i = 0; i < blockingTilesX.size(); i++)
    {
        if (tm.tileTextureIndexData[tileToCheckIndex.y * numTilesX + tileToCheckIndex.x].v[0] == blockingTilesX[i]) {
            return true;
        }
    }

    for (int i = 0; i < blockingTileIndices.size(); i++)
    {
        if (tm.tileTextureIndexData[tileToCheckIndex.y * numTilesX + tileToCheckIndex.x].v[0] == blockingTileIndices[i].x && tm.tileTextureIndexData[tileToCheckIndex.y * numTilesX + tileToCheckIndex.x].v[1] == blockingTileIndices[i].y) {
            return true;
        }
    }

    if (tileToCheckIndex.x >= totalTilesX || tileToCheckIndex.y >= totalTilesY
        || tileToCheckIndex.x < 0 || tileToCheckIndex.y < 0) {
        return true;
    }

    return false;
}