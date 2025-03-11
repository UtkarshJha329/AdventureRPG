#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 vertexTexCoord;

out vec2 texCoord;

struct CellPosInTextureAtlas{
    float x, y;
};

layout(std430, binding = 13) buffer CellPosInTextureAtlasBuffer
{
    CellPosInTextureAtlas cellPosInTextureAtlas[];
};

void main()
{
	texCoord = vertexTexCoord;
   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}