#version 450 core

layout (location = 0) in vec3 aPos;

uniform mat4 model;

#include "Common.glsl"

void main()
{
	mat4 modelViewProjection = projection * view * model;
	gl_Position = modelViewProjection * vec4(aPos, 1.0);
}