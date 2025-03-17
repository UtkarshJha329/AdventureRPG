#version 460 core

in vec2 texCoord;
flat in vec2 cameraScreenPos;
flat in vec2 cameraPosInWorld;
flat in vec2 movementInPixels;

out vec4 FragColor;

struct CellPosInTextureAtlas{
    float v[2];
};

layout(std430, binding = 3) buffer CellPosInTextureAtlasBuffer
{
    CellPosInTextureAtlas cellPosInTextureAtlas[];
};

//struct CellPosInTextureAtlas{
//    float x, y, z, w;
//};
//
//layout(std430, binding = 3) buffer CellPosInTextureAtlasBuffer
//{
//    CellPosInTextureAtlas cellPosInTextureAtlas[];
//};

uniform sampler2D ourTexture;

uniform float totalTilesX;
uniform float totalTilesY;

uniform float cellWidth;
uniform float cellHeight;

void main()
{
	vec2 offset = cameraScreenPos - movementInPixels;
	offset.y = offset.y * -1.0;

	float offsetXCoord = gl_FragCoord.x + offset.x;
	float offsetYCoord = gl_FragCoord.y + offset.y;

	float intX = floor(offsetXCoord);
	float intY = floor(offsetYCoord);

	vec2 screenResolution = vec2(1280.0, 720.0);
	float resolutionFraction = screenResolution.x / screenResolution.y;

	float numTilesX = 12.0;
	float numTilesY = numTilesX / resolutionFraction;

	float tileScaleX = screenResolution.x / numTilesX;
	float tileScaleY = screenResolution.y / numTilesY;

	float modIntX = mod(intX, tileScaleX);
	float modIntY = mod(intY, tileScaleY);

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

				vec2 offset = cameraScreenPos - movementInPixels;
				offset.y = offset.y * -1.0;

				float offsetXCoord = gl_FragCoord.x + offset.x;
				float offsetYCoord = gl_FragCoord.y + offset.y;
//				float offsetXCoord = gl_FragCoord.x + 20.0;
//				float offsetYCoord = gl_FragCoord.y + 1.0;

				float modX = mod(offsetXCoord, tileScaleX);
				float modY = mod(offsetYCoord, tileScaleY);

				vec2 calculatedTextureCoords = vec2(modX, modY);
				calculatedTextureCoords = calculatedTextureCoords / vec2(tileScaleX, tileScaleY);

				vec2 texSize = vec2(10.0, 4.0);
				float ratioOfTexDims = texSize.x / texSize.y;

				vec2 incrementCellSize = 1.0 / (texSize);
				vec2 remappedTexCoords = calculatedTextureCoords * incrementCellSize * vec2(1.0, -1.0);

				float indexCoordX = (offsetXCoord - modX) / tileScaleX;
				float indexCoordY = (offsetYCoord - modY) / tileScaleY;

				if(mod(indexCoordX, 1.0) != 0){
					indexCoordX = round(indexCoordX);
				}

				if(mod(indexCoordY, 1.0) != 0){
					indexCoordY = round(indexCoordY);
					//indexCoordY = floor(indexCoordY) + 1;
				}

//				int indexCoordX = int((intX - modIntX) / int(divideAtScaleX));
//				int indexCoordY = int((intY - modIntY) / int(divideAtScaleY));

//				indexCoordX = round(indexCoordX);
//				indexCoordY = round(indexCoordY);

				float inverseScaleOfTileX = 1 / tileScaleX;
				float inverseScaleOfTileY = 1 / tileScaleY;

				int tileDataCompressedIndex = int(indexCoordY * totalTilesX + indexCoordX);

				vec2 tileIndex = vec2(cellPosInTextureAtlas[tileDataCompressedIndex].v[0], cellPosInTextureAtlas[tileDataCompressedIndex].v[1] * -1.0);
				//tileIndex = vec2(cellPosInTextureAtlas[int(indexCoordY * totalTilesX + indexCoordX)].x / cellWidth, cellPosInTextureAtlas[int(indexCoordY * totalTilesX + indexCoordX)].y / cellHeight);
				//tileIndex = vec2(1.0, 2.0);

//				if(indexCoordX == 3 || indexCoordY == 3){
//					tileIndex = vec2(1.0, 2.0);
//				}

				vec2 remappedTexCoordsToTexture = remappedTexCoords + (tileIndex * incrementCellSize);

				//FragColor = vec4(indexCoordX / totalTilesX, indexCoordY / totalTilesY, 0.0, 1.0);
				FragColor = vec4(texture(ourTexture, remappedTexCoordsToTexture));
				//FragColor = vec4(texture(ourTexture, remappedTexCoords));
				//FragColor = vec4(texture(ourTexture, calculatedTextureCoords));
				//FragColor = vec4(1.0);
			}
		}
		else if(debugTileCoords == 1.0)
		{
			vec2 offset = cameraScreenPos - movementInPixels;
			offset.y = offset.y * -1.0;

			float offsetXCoord = gl_FragCoord.x + offset.x;
			float offsetYCoord = gl_FragCoord.y + offset.y;

			float modX = mod(offsetXCoord, tileScaleX);
			float modY = mod(offsetYCoord, tileScaleY);

			float indexCoordX = (offsetXCoord - modX) / tileScaleX;
			float indexCoordY = (offsetYCoord - modY) / tileScaleY;

//			indexCoordX = indexCoordX / numTilesX;
//			indexCoordY = indexCoordY / numTilesY;

			indexCoordX = indexCoordX / totalTilesX;
			indexCoordY = indexCoordY / totalTilesY;

			vec2 calculatedTextureCoords = vec2(indexCoordX, indexCoordY);

			FragColor = vec4(calculatedTextureCoords.x, calculatedTextureCoords.y, 0.0, 1.0);
		}
	}
	else if(debugCameraScreenPosTile == 1.0)
	{

		float distFromCamera = distance(cameraScreenPos, gl_FragCoord.xy);

		float diagonalOfCell = sqrt(pow(tileScaleX, 2.0) + pow(tileScaleY, 2.0));

		if(distFromCamera < diagonalOfCell * 0.5)
		{
			float offsetXCoord = gl_FragCoord.x;
			float offsetYCoord = gl_FragCoord.y;

			float modX = mod(offsetXCoord, tileScaleX);
			float modY = mod(offsetYCoord, tileScaleY);

			float indexCoordX = (offsetXCoord - modX) / tileScaleX;
			float indexCoordY = (offsetYCoord - modY) / tileScaleY;

			indexCoordX = cameraPosInWorld.x / (numTilesX * tileScaleX);
			indexCoordY = cameraPosInWorld.y / (numTilesY * tileScaleY);

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

		float diagonalOfCell = sqrt(pow(tileScaleX, 2.0) + pow(tileScaleY, 2.0));

		if(distFromCamera < diagonalOfCell * 0.5)
		{
			vec2 offset = vec2(cameraPosInWorld.x / (numTilesX * tileScaleX), cameraPosInWorld.y / (numTilesY * tileScaleY));

			float offsetXCoord = gl_FragCoord.x + offset.x;
			float offsetYCoord = gl_FragCoord.y + offset.y;

			float modX = mod(offsetXCoord, tileScaleX);
			float modY = mod(offsetYCoord, tileScaleY);

			vec2 calculatedTextureCoords = vec2(modX, modY);
			calculatedTextureCoords = calculatedTextureCoords / vec2(tileScaleX, tileScaleY);

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
