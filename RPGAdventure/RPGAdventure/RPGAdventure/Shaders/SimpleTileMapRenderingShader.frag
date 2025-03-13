#version 330 core

in vec2 texCoord;
flat in vec2 cameraScreenPos;
flat in vec2 cameraPosInWorld;
flat in vec2 movementInPixels;

out vec4 FragColor;

uniform sampler2D ourTexture;

void main()
{    
	float intX = floor(gl_FragCoord.x);
	float intY = floor(gl_FragCoord.y);

	vec2 screenResolution = vec2(1280.0, 720.0);
	float resolutionFraction = screenResolution.x / screenResolution.y;

	float numTilesX = 12.0;
	float numTilesY = numTilesX / resolutionFraction;

	float divideAtScaleX = 64.0;
	float divideAtScaleY = 64.0;
	
	divideAtScaleX = screenResolution.x / numTilesX;
	divideAtScaleY = screenResolution.y / numTilesY;

	float modIntX = mod(intX, divideAtScaleX);
	float modIntY = mod(intY, divideAtScaleY);

	float compareRemainder = 0.0;

	float debugCameraScreenPosTile = 0.0;

	if(debugCameraScreenPosTile == 0.0){
		float debugTileCoords = 0.0;
		if(debugTileCoords == 0.0)
		{
			if((modIntX > compareRemainder && modIntX <= compareRemainder + 0.8)|| (modIntY > compareRemainder && modIntY <= compareRemainder + 0.8))
			{
				FragColor = vec4(vec3(0.0), 1.0);
			}
			else
			{
				//vec2 offset = vec2(divideAtScaleX, divideAtScaleY) * vec2(2.0);

				vec2 offset = movementInPixels;

				float offsetXCoord = gl_FragCoord.x + offset.x;
				float offsetYCoord = gl_FragCoord.y + offset.y;
//				float offsetXCoord = gl_FragCoord.x + 20.0;
//				float offsetYCoord = gl_FragCoord.y + 1.0;

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
			vec2 offset = vec2(divideAtScaleX, divideAtScaleY) * vec2(0.0, 0.0);

			float offsetXCoord = gl_FragCoord.x + offset.x;
			float offsetYCoord = gl_FragCoord.y + offset.y;

			float modX = mod(offsetXCoord, divideAtScaleX);
			float modY = mod(offsetYCoord, divideAtScaleY);

			float indexCoordX = (offsetXCoord - modX) / divideAtScaleX;
			float indexCoordY = (offsetYCoord - modY) / divideAtScaleY;

			indexCoordX = indexCoordX / numTilesX;
			indexCoordY = indexCoordY / numTilesY;

			vec2 calculatedTextureCoords = vec2(indexCoordX, indexCoordY);

			FragColor = vec4(calculatedTextureCoords.x, calculatedTextureCoords.y, 0.0, 1.0);
		}
	}
	else if(debugCameraScreenPosTile == 1.0)
	{

		float distFromCamera = distance(cameraScreenPos, gl_FragCoord.xy);

		float diagonalOfCell = sqrt(pow(divideAtScaleX, 2.0) + pow(divideAtScaleY, 2.0));

		if(distFromCamera < diagonalOfCell * 0.5)
		{
			float offsetXCoord = gl_FragCoord.x;
			float offsetYCoord = gl_FragCoord.y;

			float modX = mod(offsetXCoord, divideAtScaleX);
			float modY = mod(offsetYCoord, divideAtScaleY);

			float indexCoordX = (offsetXCoord - modX) / divideAtScaleX;
			float indexCoordY = (offsetYCoord - modY) / divideAtScaleY;

			indexCoordX = cameraPosInWorld.x / (numTilesX * divideAtScaleX);
			indexCoordY = cameraPosInWorld.y / (numTilesY * divideAtScaleY);

			indexCoordX = indexCoordX / numTilesX;
			indexCoordY = indexCoordY / numTilesY;

			vec2 calculatedTextureCoords = vec2(indexCoordX, indexCoordY);

			FragColor = vec4(calculatedTextureCoords.x, calculatedTextureCoords.y, 0.0, 1.0);
			//FragColor = vec4(texture(ourTexture, calculatedTextureCoords));

		}
	}
	else if(debugCameraScreenPosTile == 2.0)
	{
		float distFromCamera = distance(cameraScreenPos, gl_FragCoord.xy);

		float diagonalOfCell = sqrt(pow(divideAtScaleX, 2.0) + pow(divideAtScaleY, 2.0));

		if(distFromCamera < diagonalOfCell * 0.5)
		{
			vec2 offset = vec2(cameraPosInWorld.x / (numTilesX * divideAtScaleX), cameraPosInWorld.y / (numTilesY * divideAtScaleY));

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

		}

	}
}
