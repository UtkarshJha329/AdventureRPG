#version 330 core

in vec2 texCoord;

out vec4 FragColor;

uniform sampler2D ourTexture;

void main()
{

    vec2 tileMapScale = vec2(10.0, 10.0);
    vec2 tileMapSpriteSheetWH = vec2(10.0, 4.0);
    vec2 tileTextureWH = vec2(64.0, 64.0);

    vec2 cellSizeForSpriteSheet = 1 / tileMapSpriteSheetWH;

    vec2 curTileIndex = vec2(1.0, 1.0);

    vec2 moddedTexCoord = vec2(mod(texCoord.x, cellSizeForSpriteSheet.x), mod(texCoord.y, cellSizeForSpriteSheet.y));

    vec2 scaledByCellIndexInSpriteSheet = moddedTexCoord + (curTileIndex * cellSizeForSpriteSheet);

    FragColor = vec4(texture(ourTexture, scaledByCellIndexInSpriteSheet));
    //FragColor = vec4(texture(ourTexture, moddedTexCoord));
    //FragColor = vec4(texture(ourTexture, texCoord));
    //FragColor = vec4(1.0, 1.0, 0.0, 1.0);
}
