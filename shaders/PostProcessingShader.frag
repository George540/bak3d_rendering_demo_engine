#version 450 core

in vec2 TextCoords;

out vec4 FragColor;

uniform sampler2D screenTexture;

void main()
{
    FragColor = vec4(vec3(1.0 - texture(screenTexture, TextCoords)), 1.0);
}
