#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 worldMatrix;
uniform mat4 viewMatrix = mat4(1.0);
uniform mat4 projectionMatrix = mat4(1.0);

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;

void main()
{
	mat4 modelViewProjection = projectionMatrix * viewMatrix * worldMatrix;
	gl_Position = modelViewProjection * vec4(aPos, 1.0);
}