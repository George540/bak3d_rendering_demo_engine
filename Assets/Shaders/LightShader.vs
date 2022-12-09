#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view = mat4(1.0);
uniform mat4 projection = mat4(1.0);

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;

void main()
{
	mat4 modelViewProjection = projection * view * model;
	gl_Position = modelViewProjection * vec4(aPos, 1.0);
}