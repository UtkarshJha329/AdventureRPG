#pragma once

#include <string>

class TextureResource {

public:

	std::string texSrc = "";
	unsigned int numSpriteCellsX = 1;
	unsigned int numSpriteCellsY = 1;

	unsigned int paddingX = 0;
	unsigned int paddingY = 0;
};