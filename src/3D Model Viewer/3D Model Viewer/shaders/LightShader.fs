#version 330 core

uniform vec4 diffuseColor = vec4(1.0);

out vec4 FragColor;

void main()
{
	FragColor = diffuseColor;
}