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
//	float PI = 3.141592;
//	float N = 20.0;
    
	float intX = floor(gl_FragCoord.x);
	float intY = floor(gl_FragCoord.y);

//	vec2 screenResolution = vec2(800.0, 450.0);
//	float resolutionFraction = screenResolution.x / screenResolution.y;
	
//	vec2 numTilesOnScreen = vec2(12.5, 7.03125);
//	numTilesOnScreen = vec2(20, 10);
//	numTilesOnScreen.y = numTilesOnScreen.y * resolutionFraction;
//
//	vec2 divisionByPixels = screenResolution / numTilesOnScreen;
//
//	float divideAtScaleX = divisionByPixels.x;
//	float divideAtScaleY = divisionByPixels.y;

	float numTilesOneGrid = pow(2.0, 1.0);

	//vec2 screenResolution = vec2(800.0, 450.0);
	vec2 screenResolution = vec2(1280.0, 720.0);
	float resolutionFraction = screenResolution.x / screenResolution.y;

	float numTilesX = 12.0;
	float numTilesY = numTilesX / resolutionFraction;

	float divideAtScaleX = 64.0;
	float divideAtScaleY = 64.0;
	
//	divideAtScaleX = divideAtScaleX / numTilesOneGrid;
//	divideAtScaleY = divideAtScaleY / numTilesOneGrid;

	divideAtScaleX = screenResolution.x / numTilesX;
	divideAtScaleY = screenResolution.y / numTilesY;

	float modIntX = mod(intX, divideAtScaleX);
	float modIntY = mod(intY, divideAtScaleY);

	float compareRemainder = 0.0;

	float debugTileCoords = 1.0;
	if(debugTileCoords == 0.0)
	{
		if((modIntX > compareRemainder && modIntX <= compareRemainder + 0.8)|| (modIntY > compareRemainder && modIntY <= compareRemainder + 0.8))
		{
			FragColor = vec4(vec3(0.0), 1.0);
		}
		else
		{
			vec2 offset = vec2(divideAtScaleX, divideAtScaleY) * vec2(0.0, 0.0);

			float offsetXCoord = gl_FragCoord.x + offset.x;
			float offsetYCoord = gl_FragCoord.y + offset.y;

			float modX = mod(offsetXCoord, divideAtScaleX);
			float modY = mod(offsetYCoord, divideAtScaleY);

			vec2 calculatedTextureCoords = vec2(modX, modY);
			calculatedTextureCoords = calculatedTextureCoords / vec2(divideAtScaleX, divideAtScaleY);

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
	else if(debugTileCoords == 1.0)
	{
//		divideAtScaleX = 64.0;
//		divideAtScaleY = 64.0;

//		float x = gl_FragCoord.x / divideAtScaleX;
//		float y = gl_FragCoord.y / divideAtScaleY;

		vec2 offset = vec2(divideAtScaleX, divideAtScaleY) * vec2(0.0, 0.0);

		float offsetXCoord = gl_FragCoord.x + offset.x;
		float offsetYCoord = gl_FragCoord.y + offset.y;

		float modX = mod(offsetXCoord, divideAtScaleX);
		float modY = mod(offsetYCoord, divideAtScaleY);

		float indexCoordX = (offsetXCoord - modX) / divideAtScaleX;
		float indexCoordY = (offsetYCoord - modY) / divideAtScaleY;

		indexCoordX = indexCoordX / numTilesX;
		indexCoordY = indexCoordY / numTilesY;

		//vec2 calculatedTextureCoords = vec2(modX, modY);
		vec2 calculatedTextureCoords = vec2(indexCoordX, indexCoordY);
		//calculatedTextureCoords = calculatedTextureCoords / vec2(divideAtScaleX, divideAtScaleY);

//		float divX = offsetXCoord / divideAtScaleX;
//		float divY = offsetYCoord / divideAtScaleY;
//
//		divX = divX / numTilesX;
//		divY = divY / numTilesY;
//		
//		float modCompartmentalisedX = modX / numTilesX;
//		float modCompartmentalisedY = modY / numTilesY;

		//FragColor = vec4(gl_FragCoord.x, gl_FragCoord.y, 0.0, 1.0);
		//FragColor = vec4(x, y, 0.0, 1.0);
		//FragColor = vec4(modX, modY, 0.0, 1.0);
		//FragColor = vec4(divX, divY, 0.0, 1.0);
		//FragColor = vec4(modCompartmentalisedX, modCompartmentalisedY, 0.0, 1.0);
		FragColor = vec4(calculatedTextureCoords.x, calculatedTextureCoords.y, 0.0, 1.0);
	}
}
