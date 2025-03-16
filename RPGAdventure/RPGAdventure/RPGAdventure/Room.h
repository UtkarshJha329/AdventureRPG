//#pragma once
//
//#include <vector>
//
//#include "flecs/flecs.h"
//#include "raylib/raylib.h"
//
//const int roomHalfWidth = 6;
//
//constexpr float screenResolutionX = 1280.0f;
//constexpr float screenResolutionY = 720.0f;
//constexpr float resolutionFraction = screenResolutionX / screenResolutionY;
//
//constexpr float numTilesX = roomHalfWidth * 2.0f;
//constexpr float numTilesY = numTilesX / resolutionFraction;
//
//constexpr float tileScaleX = screenResolutionX / numTilesX;
//constexpr float tileScaleY = screenResolutionY / numTilesY;
//
//constexpr float roomHalfWidthInPixels = roomHalfWidth * tileScaleX;
//constexpr float roomHalfHeightInPixels = numTilesY * 0.5f * tileScaleY;
//
//constexpr float cameraHalfWidthInPix = 10.0f * 0.5f;
//constexpr float cameraHalfHeightInPix = 10.0f * 0.5f;
//
//class Room {
//
//public:
//	std::vector<flecs::entity> entitiesInThisRoom;
//};
//
//Vector2 CurrentTileCoordIndex(Vector2 pointPos) {
//
//	float modX = std::fmod(pointPos.x, tileScaleX);
//	float modY = std::fmod(pointPos.y, tileScaleY);
//
//	float indexCoordX = (pointPos.x - modX) / tileScaleX;
//	float indexCoordY = (pointPos.y - modY) / tileScaleY;
//
//	return Vector2{ indexCoordX, indexCoordY };
//}
//
//Vector2 CurrentRoomIndex(Vector2 pointPos) {
//
//	float totalTilesX = 100;
//	float totalTilesY = 100;
//	
//	Vector2 tileIndexCoords = CurrentTileCoordIndex(pointPos);
//
//	int roomIndexCoordsX = tileIndexCoords.x / numTilesX;
//	int roomIndexCoordsY = tileIndexCoords.y / numTilesY;
//
//	return Vector2{ (float)roomIndexCoordsX, (float)roomIndexCoordsY };
//}
//
//bool PointLiesInsideRoom(Vector2 roomIndex, Vector2 point, Camera2D& camera, bool convertPointToScreenCoords = true, bool printDebug = false) {
//
//	Vector2 roomPosition = Vector2{ 0.0f, 0.0f };
//	Vector2 roomPosInScreenCoords = GetWorldToScreen2D(roomPosition, camera);
//
//	Vector2 topRightOfRoom = Vector2{ roomPosInScreenCoords.x + roomHalfWidthInPixels, roomPosInScreenCoords.y + roomHalfHeightInPixels };
//	Vector2 bottomRightOfRoom = Vector2{ roomPosInScreenCoords.x + roomHalfWidthInPixels, roomPosInScreenCoords.y - roomHalfHeightInPixels };
//	Vector2 topLeftOfRoom = Vector2{ roomPosInScreenCoords.x - roomHalfWidthInPixels, roomPosInScreenCoords.y + roomHalfHeightInPixels };
//	Vector2 bottomLeftOfRoom = Vector2{ roomPosInScreenCoords.x - roomHalfWidthInPixels, roomPosInScreenCoords.y - roomHalfHeightInPixels };
//
//	if (convertPointToScreenCoords) {
//		point = GetWorldToScreen2D(point, camera);
//	}
//
//	if (printDebug) {
//		std::cout << "Room pos in screen coords : "
//			<< roomPosInScreenCoords.x << ", " << roomPosInScreenCoords.y
//			<< "\n\t Top Right := " << topRightOfRoom.x << ", " << topRightOfRoom.y
//			<< "\n\t Bottom Right := " << bottomRightOfRoom.x << ", " << bottomRightOfRoom.y
//			<< "\n\t Top Left := " << topLeftOfRoom.x << ", " << topLeftOfRoom.y
//			<< "\n\t Bottom Left := " << bottomLeftOfRoom.x << ", " << bottomLeftOfRoom.y
//			<< "\n\t Point :=" << point.x << ", " << point.y
//			<< std::endl;
//	}
//
//	return (point.x < topRightOfRoom.x && point.y < topRightOfRoom.y)
//		&& (point.x < bottomRightOfRoom.x && point.y > bottomRightOfRoom.y)
//		&& (point.x > topLeftOfRoom.x && point.y < topLeftOfRoom.y)
//		&& (point.x > bottomLeftOfRoom.x && point.y > bottomLeftOfRoom.y);
//
//}
//
//
//bool CanCameraMoveInRoom(Vector2 roomIndex, Vector2 cameraPos, Camera2D& camera) {
//
//	cameraPos = GetWorldToScreen2D(cameraPos, camera);
//
//	Vector2 cameraTopRight = Vector2{ cameraPos.x + cameraHalfWidthInPix, cameraPos.y + cameraHalfHeightInPix };
//	Vector2 cameraBottomRight = Vector2{ cameraPos.x + cameraHalfWidthInPix, cameraPos.y - cameraHalfHeightInPix };
//	Vector2 cameraTopLeft = Vector2{ cameraPos.x - cameraHalfWidthInPix, cameraPos.y + cameraHalfHeightInPix };
//	Vector2 cameraBottomLeft = Vector2{ cameraPos.x - cameraHalfWidthInPix, cameraPos.y - cameraHalfHeightInPix };
//
//	return PointLiesInsideRoom(roomIndex, cameraTopRight, camera, false)
//		&& PointLiesInsideRoom(roomIndex, cameraBottomRight, camera, false)
//		&& PointLiesInsideRoom(roomIndex, cameraTopLeft, camera, false)
//		&& PointLiesInsideRoom(roomIndex, cameraBottomLeft, camera, false);
//
//	return false;
//}

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

class Room {

public:
	std::vector<flecs::entity> entitiesInThisRoom;
};

Vector2 CurrentTileCoordIndex(Vector2 pointScreenPos, Vector2 cameraScreenPos, Vector2 movementInPixels) {

	Vector2 offset = cameraScreenPos - movementInPixels;

	pointScreenPos = pointScreenPos + offset;

	float modX = std::fmod(pointScreenPos.x, tileScaleX);
	float modY = std::fmod(pointScreenPos.y, tileScaleY);

	float indexCoordX = (pointScreenPos.x - modX) / tileScaleX;
	float indexCoordY = (pointScreenPos.y - modY) / tileScaleY;

	return Vector2{ indexCoordX, indexCoordY };
}

Vector2 CurrentRoomIndex(Vector2 pointScreenPos, Vector2 cameraScreenPos, Vector2 movementInPixels, Vector2 offsetByTiles) {

	float totalTilesX = 100;
	float totalTilesY = 100;

	Vector2 tileIndexCoords = CurrentTileCoordIndex(pointScreenPos, cameraScreenPos, movementInPixels);

	//std::cout << "TILE INDEX COORDS := " << tileIndexCoords.x << ", " << tileIndexCoords.y << std::endl;

	float modX = std::fmod(tileIndexCoords.x + offsetByTiles.x, numTilesX);
	float modY = std::fmod(tileIndexCoords.y + offsetByTiles.y, numTilesY);

	float roomIndexCoordsX = (tileIndexCoords.x - modX) / numTilesX;
	float roomIndexCoordsY = (tileIndexCoords.y - modY) / numTilesY;

	return Vector2{ (float)roomIndexCoordsX, (float)roomIndexCoordsY * -1.0f };
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