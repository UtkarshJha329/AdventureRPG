#pragma once

#include <iostream>
#include <string>

#include "raylib/raylib.h"

class SpriteSheet {

public:

	Texture2D spriteSheetTexture;

	Rectangle cell = { 0.0f, 0.0f, 0.0f, 0.0f };

	unsigned int numSpriteCellsX;
	unsigned int numSpriteCellsY;

    unsigned int paddingX;
    unsigned int paddingY;
};

float SpriteSheetCellWidth(SpriteSheet& spriteSheet) {
    return spriteSheet.spriteSheetTexture.width / spriteSheet.numSpriteCellsX;
}

float SpriteSheetCellHeight(SpriteSheet& spriteSheet) {
    return spriteSheet.spriteSheetTexture.height / spriteSheet.numSpriteCellsY;
}

void InitSpriteSheet(SpriteSheet& spriteSheet, std::string textureLocation, unsigned int _numSpriteCellsX, unsigned int _numSpriteCellsY, unsigned int _paddingX, unsigned int _paddingY) {

    //std::cout << "Loading : " << textureLocation << std::endl;

    spriteSheet.spriteSheetTexture = LoadTexture(textureLocation.c_str());

    spriteSheet.numSpriteCellsX = _numSpriteCellsX;
    spriteSheet.numSpriteCellsY = _numSpriteCellsY;

    spriteSheet.paddingX = _paddingX;
    spriteSheet.paddingY = _paddingY;

    float spriteWidth = SpriteSheetCellWidth(spriteSheet);
    float spriteHeight = SpriteSheetCellHeight(spriteSheet);

    //spriteSheet.cell = { 0.0f + spriteSheet.paddingX, 0.0f + spriteSheet.paddingY, spriteWidth, spriteHeight };
    //spriteSheet.cell = { -spriteWidth * 0.5F, -spriteHeight * 0.5f, spriteWidth, spriteHeight };
    spriteSheet.cell = { 0.0f, 0.0f, spriteWidth, spriteHeight };
    //spriteSheet.cell = { 0.0f + spriteSheet.paddingX, 0.0f + spriteSheet.paddingY, spriteWidth, spriteHeight};
}