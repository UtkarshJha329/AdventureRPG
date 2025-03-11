#pragma once

#include <string>

#include "raylib/raylib.h"

class SpriteSheet {

public:

	Texture2D spriteSheetTexture;

	Rectangle cell = { 0.0f, 0.0f, 0.0f, 0.0f };

	unsigned int numSpriteCellsX;
	unsigned int numSpriteCellsY;
};

float SpriteSheetCellWidth(SpriteSheet& spriteSheet) {
    return spriteSheet.spriteSheetTexture.width / spriteSheet.numSpriteCellsX;
}

float SpriteSheetCellHeight(SpriteSheet& spriteSheet) {
    return spriteSheet.spriteSheetTexture.height / spriteSheet.numSpriteCellsY;
}

void InitSpriteSheet(SpriteSheet& spriteSheet, std::string textureLocation, unsigned int _numSpriteCellsX, unsigned int _numSpriteCellsY) {
    spriteSheet.spriteSheetTexture = LoadTexture(textureLocation.c_str());

    spriteSheet.numSpriteCellsX = _numSpriteCellsX;
    spriteSheet.numSpriteCellsY = _numSpriteCellsY;

    spriteSheet.cell = { 0.0f, 0.0f, SpriteSheetCellWidth(spriteSheet), SpriteSheetCellHeight(spriteSheet) };
}