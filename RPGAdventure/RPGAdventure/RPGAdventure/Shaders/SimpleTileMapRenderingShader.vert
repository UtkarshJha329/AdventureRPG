#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 vertexTexCoord;

out vec2 texCoord;
flat out vec2 cameraScreenPos;
flat out vec2 cameraPosInWorld;
flat out vec2 movementInPixels;

uniform vec2 cameraPosOnScreen;
uniform vec2 cameraPos;
uniform vec2 pixelMovement;

void main()
{
    cameraScreenPos = cameraPosOnScreen;
    cameraPosInWorld = cameraPos;
    movementInPixels = pixelMovement;

	texCoord = vertexTexCoord;
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}