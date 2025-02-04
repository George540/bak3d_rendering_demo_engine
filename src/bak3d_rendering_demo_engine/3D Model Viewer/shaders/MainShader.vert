#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 worldMatrix = mat4(1.0f);
uniform mat4 viewMatrix = mat4(1.0);
uniform mat4 projectionMatrix = mat4(1.0);

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;

void main()
{
	FragPos = vec3(worldMatrix * vec4(aPos, 1.0));
	Normal = mat3(transpose(inverse(worldMatrix))) * aNormal;

	mat4 modelViewProjection = projectionMatrix * viewMatrix * worldMatrix;
	gl_Position = modelViewProjection * vec4(aPos, 1.0);

	TexCoord = aTexCoord;
}