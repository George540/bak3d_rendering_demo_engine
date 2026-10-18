#version 460 core

layout (location = 0) in vec3 aPos;

out vec3 TexDir;

uniform mat3 sky_rotation = mat3(1.0); // inverse of the user rotation

#include "Common_Global.glsl"

void main()
{
    TexDir = sky_rotation * aPos; // rotate only the sample direction
    mat4 view_rotation = mat4(mat3(camera_data.view)); // strip translation
    vec4 clip = camera_data.projection * view_rotation * vec4(aPos, 1.0);
    gl_Position = clip.xyww;
}