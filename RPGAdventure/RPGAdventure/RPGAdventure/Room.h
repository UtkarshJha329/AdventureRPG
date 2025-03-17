#pragma once

#include <vector>

#include "flecs/flecs.h"
#include "raylib/raylib.h"

const int roomHalfWidth = 6;

constexpr float screenResolutionX = 1280.0f;
constexpr float screenResolutionY = 720.0f;
constexpr float resolutionFraction = screenResolutionX / screenResolutionY;

constexpr float numTilesX = roomHalfWidth * 2.0f;
constexpr float numTilesY = numTilesX / resolutionFraction;

constexpr float tileScaleX = screenResolutionX / numTilesX;
constexpr float tileScaleY = screenResolutionY / numTilesY;

constexpr float roomHalfWidthInPixels = roomHalfWidth * tileScaleX;
constexpr float roomHalfHeightInPixels = numTilesY * 0.5f * tileScaleY;

constexpr float cameraHalfWidthInPix = 10.0f * 0.5f;
constexpr float cameraHalfHeightInPix = 10.0f * 0.5f;

constexpr float totalRoomsX = 3.0f;
constexpr float totalRoomsY = 10.0f;

constexpr float totalTilesX = (float)((int)(totalRoomsX * numTilesX));
constexpr float totalTilesY = (float)((int)(totalRoomsY * numTilesY));

//constexpr float totalTilesX = 36.0f;
//constexpr float totalTilesY = 36.0f;

class Room {

public:
	std::vector<flecs::entity> torchGoblinEntitiesInThisRoom;
};

Vector2 CurrentTileCoordIndex(Vector2 pointScreenPos, Vector2 cameraScreenPos, Vector2 movementInPixels) {

	Vector2 offset = cameraScreenPos - movementInPixels;

	pointScreenPos = pointScreenPos + offset - Vector2{ 0.0f, numTilesY * tileScaleY };

	float modX = std::fmod(pointScreenPos.x, tileScaleX);
	float modY = std::fmod(pointScreenPos.y, tileScaleY);

	float indexCoordX = (pointScreenPos.x - modX) / tileScaleX;
	float indexCoordY = (pointScreenPos.y - modY) / tileScaleY;

	if (std::fmod(indexCoordX, 1.0) != 0) {
		indexCoordX = round(indexCoordX);
	}

	if (std::fmod(indexCoordY, 1.0) != 0) {
		indexCoordY = round(indexCoordY);
		//indexCoordY = floor(indexCoordY) + 1;
	}

	return Vector2{ indexCoordX, indexCoordY * -1.0f };
}

Vector2 CurrentRoomIndex(Vector2 pointScreenPos, Vector2 cameraScreenPos, Vector2 movementInPixels, Vector2 offsetByTiles = Vector2Zeros) {

	Vector2 tileIndexCoords = CurrentTileCoordIndex(pointScreenPos, cameraScreenPos, movementInPixels);
	//std::cout << "TILE INDEX COORDS := " << tileIndexCoords.x << ", " << tileIndexCoords.y << std::endl;

	float modX = std::fmod(tileIndexCoords.x + offsetByTiles.x, numTilesX);
	float modY = std::fmod(tileIndexCoords.y + offsetByTiles.y, numTilesY);

	float roomIndexCoordsX = (tileIndexCoords.x - modX) / numTilesX;
	float roomIndexCoordsY = (tileIndexCoords.y - modY) / numTilesY;

	//return Vector2{ (float)roomIndexCoordsX, (float)roomIndexCoordsY * -1.0f };
	return Vector2{ (float)roomIndexCoordsX, (float)roomIndexCoordsY};
}

bool PointLiesInsideRoom(Vector2 roomIndex, Vector2 point, Vector2 cameraScreenPos, Vector2 movementInPixels, Vector2 offsetByTiles) {

	Vector2 curPointRoomIndex = CurrentRoomIndex(point, cameraScreenPos, movementInPixels, offsetByTiles);
	return (roomIndex.x == curPointRoomIndex.x && roomIndex.y == curPointRoomIndex.y);
}

Vector2 CameraPosForRoom(Vector2 roomIndex, Camera2D& camera) {

	return GetScreenToWorld2D(roomIndex * Vector2{ numTilesX * tileScaleX, numTilesY * tileScaleY }, camera);

}

bool CanCameraMoveInRoom(Vector2 roomIndex, Vector2 cameraScreenPos, Vector2 movementInPixels, Vector2 offsetByTiles) {

	float screenResPercentageBox = 0.25f;
	Vector2 cameraBoxHalfWidthInPixels = Vector2{ screenResolutionX * screenResPercentageBox, screenResolutionY * screenResPercentageBox };

	cameraScreenPos = cameraScreenPos + roomIndex * Vector2{ numTilesX * tileScaleX, numTilesY * tileScaleY};

	Vector2 cameraBottomLeftScreenPos = cameraScreenPos - cameraBoxHalfWidthInPixels;
	Vector2 cameraBottomRightScreenPos = Vector2{ cameraScreenPos.x + cameraBoxHalfWidthInPixels.x, cameraScreenPos.y - cameraBoxHalfWidthInPixels.y };
	Vector2 cameraTopRightScreenPos = cameraScreenPos + cameraBoxHalfWidthInPixels;
	Vector2 cameraTopLeftScreenPos = Vector2{ cameraScreenPos.x - cameraBoxHalfWidthInPixels.x, cameraScreenPos.y + cameraBoxHalfWidthInPixels.y };

	return PointLiesInsideRoom(roomIndex, cameraBottomLeftScreenPos, cameraScreenPos, movementInPixels, offsetByTiles)
		&& PointLiesInsideRoom(roomIndex, cameraBottomRightScreenPos, cameraScreenPos, movementInPixels, offsetByTiles)
		&& PointLiesInsideRoom(roomIndex, cameraTopRightScreenPos, cameraScreenPos, movementInPixels, offsetByTiles)
		&& PointLiesInsideRoom(roomIndex, cameraTopLeftScreenPos, cameraScreenPos, movementInPixels, offsetByTiles);
}