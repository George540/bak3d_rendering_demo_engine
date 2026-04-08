#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aCol;

uniform mat4 model;

#include "Common.glsl"

out vec3 vertexColor;

void main()
{
	vertexColor = aCol;
	mat4 modelViewProjection = projection * view * model;
	gl_Position = modelViewProjection * vec4(aPos, 1.0);
}