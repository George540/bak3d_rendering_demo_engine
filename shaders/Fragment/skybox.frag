#version 460 core

in vec3 TexDir;
out vec4 FragColor;

uniform samplerCube skybox;

void main()
{
    FragColor = vec4(texture(skybox, TexDir).rgb, 1.0);
}
