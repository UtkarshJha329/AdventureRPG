#version 330 core

in vec2 texCoord;

out vec4 FragColor;

uniform sampler2D ourTexture;

//void main()
//{
//
//        vec2 windowResolution = vec2(800.0, 450.0);
//        //vec2 windowResolution = vec2(1024.0, 720.0);
//        float resolutionFraction = windowResolution.x / windowResolution.y;
//        //float resolutionFraction = windowResolution.y / windowResolution.x;
//        vec2 resolutionAccurateTexCoords = texCoord * vec2(1.0, resolutionFraction);
//        
//        //vec2 normalisedFragCoord = gl_FragCoord.xy / windowResolution;
//        //vec2 resolutionAccurateTexCoords = normalisedFragCoord * vec2(1.0, resolutionFraction);
//        //vec2 resolutionAccurateTexCoords = normalisedFragCoord;
//
//        vec2 tileMapScale = vec2(1.0, 1.0);
//        vec2 tileMapSpriteSheetWH = vec2(10.0, 4.0);
//
//        vec2 cellSizeForSpriteSheet = 1 / (tileMapSpriteSheetWH);
//
//        //vec2 curTileIndex = vec2(0.0, 0.0);
//        vec2 curTileIndex = vec2(1.0, 2.0);
//        //vec2 curTileIndex = vec2(1.0, 1.0);
//        //vec2 curTileIndex = vec2(2.0, 1.0);
//
//        vec2 moddedTexCoord = vec2(mod(resolutionAccurateTexCoords.x, cellSizeForSpriteSheet.x), 1.0 - mod(resolutionAccurateTexCoords.y, cellSizeForSpriteSheet.y));
//
//        vec2 scaledByCellIndexInSpriteSheet = moddedTexCoord + (curTileIndex * cellSizeForSpriteSheet * vec2(1.0, -1.0));
//        //vec2 scaledByCellIndexInSpriteSheet = moddedTexCoord + (curTileIndex * cellSizeForSpriteSheet * vec2(1.0, 1.0));
//
//
//
//        //FragColor = vec4(resolutionAccurateTexCoords, 0.0, 1.0);
//        FragColor = vec4(texture(ourTexture, scaledByCellIndexInSpriteSheet));
//        //FragColor = vec4(gl_FragCoord.x / windowResolution.x, gl_FragCoord.y / windowResolution.y, 0.0, 1.0);
//        //FragColor = vec4(1.0, 1.0, 0.0, 1.0);
//}
//

void main()
{
	float PI = 3.141592;
	float N = 20.0;

	vec2 screenResolution = vec2(800.0, 450.0);
	vec2 mappedFragCoord = gl_FragCoord.xy;
    
	float intX = floor(gl_FragCoord.x);
	float intY = floor(gl_FragCoord.y);

	float divideAtScale = 64.0;
	divideAtScale = divideAtScale / 2.0;

	float modIntX = mod(intX, divideAtScale);
	float modIntY = mod(intY, divideAtScale);

	if(modIntX == 0 || modIntY == 0)
	{
		FragColor = vec4(vec3(0.0), 1.0);
	}
	else
	{
		float modX = mod(gl_FragCoord.x, divideAtScale);
		float modY = mod(gl_FragCoord.y, divideAtScale);

		vec2 calculatedTextureCoords = vec2(modX, modY);
		calculatedTextureCoords = calculatedTextureCoords / divideAtScale;

		vec2 texSize = vec2(10.0, 4.0);
		float ratioOfTexDims = texSize.x / texSize.y;

		vec2 incrementCellSize = 1.0 / (texSize);
		vec2 remappedTexCoords = calculatedTextureCoords * incrementCellSize * vec2(1.0, -1.0);

		vec2 tileIndex = vec2(1.0, 2.0);

		vec2 remappedTexCoordsToTexture = remappedTexCoords + (tileIndex * incrementCellSize);

		FragColor = vec4(texture(ourTexture, remappedTexCoordsToTexture));
		//FragColor = vec4(texture(ourTexture, remappedTexCoords));
		//FragColor = vec4(texture(ourTexture, calculatedTextureCoords));
		//FragColor = vec4(1.0);
	}

}
