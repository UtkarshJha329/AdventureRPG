#version 330 core

in vec2 texCoord;

out vec4 FragColor;

uniform sampler2D ourTexture;

void main()
{
    vec2 windowResolution = vec2(800.0, 450.0);
    float resolutionFraction = windowResolution.x / windowResolution.y;
    vec2 resolutionAccurateTexCoords = texCoord * vec2(1.0, resolutionFraction);

    vec2 tileMapScale = vec2(1.0, 1.0);
    vec2 tileMapSpriteSheetWH = vec2(10.0, 4.0);

    vec2 cellSizeForSpriteSheet = 1 / (tileMapSpriteSheetWH);

    //vec2 curTileIndex = vec2(0.0, 0.0);
    vec2 curTileIndex = vec2(1.0, 2.0);
    //vec2 curTileIndex = vec2(1.0, 1.0);
    //vec2 curTileIndex = vec2(2.0, 1.0);

    vec2 moddedTexCoord = vec2(mod(resolutionAccurateTexCoords.x, cellSizeForSpriteSheet.x), 1.0 - mod(resolutionAccurateTexCoords.y, cellSizeForSpriteSheet.y));

    vec2 scaledByCellIndexInSpriteSheet = moddedTexCoord + (curTileIndex * cellSizeForSpriteSheet * vec2(1.0, -1.0));

    FragColor = vec4(texture(ourTexture, scaledByCellIndexInSpriteSheet));
    //FragColor = vec4(1.0, 1.0, 0.0, 1.0);
}
