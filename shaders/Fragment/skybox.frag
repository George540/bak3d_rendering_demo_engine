#version 460 core

in vec3 TexDir;
out vec4 FragColor;

uniform samplerCube skybox;
uniform vec3 tint = vec3(1.0);
uniform float intensity = 1.0;

void main()
{
    vec3 sky_color = texture(skybox, TexDir).rgb;
    FragColor = vec4(sky_color * tint * intensity, 1.0);
}
