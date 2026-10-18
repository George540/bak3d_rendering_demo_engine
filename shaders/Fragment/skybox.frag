#version 460 core

in vec3 TexDir;
out vec4 FragColor;

uniform samplerCube cubemap;
uniform vec3 color = vec3(1.0);
uniform float intensity = 1.0;

void main()
{
    vec3 sky_color = texture(cubemap, TexDir).rgb;
    FragColor = vec4(sky_color * color * intensity, 1.0);
}
