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

class Room {

public:
	std::vector<flecs::entity> entitiesInThisRoom;
};

bool PointLiesInsideRoom(Vector2 roomPosition, Vector2 point, Camera2D& camera, bool convertPointToScreenCoords = true) {

	Vector2 roomPosInScreenCoords = GetWorldToScreen2D(roomPosition, camera);

	Vector2 topRightOfRoom = Vector2{ roomPosInScreenCoords.x + roomHalfWidthInPixels, roomPosInScreenCoords.y + roomHalfHeightInPixels };
	Vector2 bottomRightOfRoom = Vector2{ roomPosInScreenCoords.x + roomHalfWidthInPixels, roomPosInScreenCoords.y - roomHalfHeightInPixels };
	Vector2 topLeftOfRoom = Vector2{ roomPosInScreenCoords.x - roomHalfWidthInPixels, roomPosInScreenCoords.y + roomHalfHeightInPixels };
	Vector2 bottomLeftOfRoom = Vector2{ roomPosInScreenCoords.x - roomHalfWidthInPixels, roomPosInScreenCoords.y - roomHalfHeightInPixels };

	if (convertPointToScreenCoords) {
		point = GetWorldToScreen2D(point, camera);
	}

	return (point.x < topRightOfRoom.x && point.y < topRightOfRoom.y)
		&& (point.x < bottomRightOfRoom.x && point.y > bottomRightOfRoom.y)
		&& (point.x > topLeftOfRoom.x && point.y < topLeftOfRoom.y)
		&& (point.x > bottomLeftOfRoom.x && point.y > bottomLeftOfRoom.y);

}


bool CanCameraMoveInRoom(Vector2 roomPosition, Vector2 cameraPos, Camera2D& camera) {

	float cameraHalfWidthInPix = 800.0f * 0.5f;
	float cameraHalfHeightInPix = 450.0f * 0.5f;

	cameraPos = GetWorldToScreen2D(cameraPos, camera);

	Vector2 cameraTopRight = Vector2{ cameraPos.x + cameraHalfWidthInPix, cameraPos.y + cameraHalfHeightInPix };
	Vector2 cameraBottomRight = Vector2{ cameraPos.x + cameraHalfWidthInPix, cameraPos.y - cameraHalfHeightInPix };
	Vector2 cameraTopLeft = Vector2{ cameraPos.x - cameraHalfWidthInPix, cameraPos.y + cameraHalfHeightInPix };
	Vector2 cameraBottomLeft = Vector2{ cameraPos.x - cameraHalfWidthInPix, cameraPos.y - cameraHalfHeightInPix };

	//std::cout << "cameraTopRight := " << cameraTopRight.x << ", " << cameraTopRight.y << std::endl;
	//std::cout << "cameraBottomLeft := " << cameraBottomLeft.x << ", " << cameraBottomLeft.y << std::endl;

	//bool returnVal = (point.x < topRightOfRoom.x && point.y < topRightOfRoom.y)
	//	&& (point.x < bottomRightOfRoom.x && point.y > bottomRightOfRoom.y)
	//	&& (point.x > topLeftOfRoom.x && point.y < topLeftOfRoom.y)
	//	&& (point.x > bottomLeftOfRoom.x && point.y > bottomLeftOfRoom.y);

	//if ((point.x < topRightOfRoom.x && point.y < topRightOfRoom.y)) {
	//	if ((point.x < bottomRightOfRoom.x && point.y > bottomRightOfRoom.y)) {
	//		if ((point.x > topLeftOfRoom.x && point.y < topLeftOfRoom.y)) {
	//			if ((point.x > bottomLeftOfRoom.x && point.y > bottomLeftOfRoom.y)) {
	//				return true;
	//			}
	//			else {
	//				std::cout << "outside bottom left" << std::endl;
	//			}
	//		}
	//		else {
	//			std::cout << "outside top left" << std::endl;
	//		}
	//	}
	//	else {
	//		std::cout << "outside bottom right" << std::endl;
	//	}
	//}
	//else {
	//	std::cout << "outside top right" << std::endl;
	//}


	//std::cout << returnVal << std::endl;

	//return returnVal;

	return PointLiesInsideRoom(roomPosition, cameraTopRight, camera, false)
		&& PointLiesInsideRoom(roomPosition, cameraBottomRight, camera, false)
		&& PointLiesInsideRoom(roomPosition, cameraTopLeft, camera, false)
		&& PointLiesInsideRoom(roomPosition, cameraBottomLeft, camera, false);

	return false;
}