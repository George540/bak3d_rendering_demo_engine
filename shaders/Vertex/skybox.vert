#version 460 core

layout (location = 0) in vec3 aPos;

out vec3 TexDir;

#include "Common_Global.glsl"

void main()
{
    TexDir = aPos;
    mat4 view_rotation = mat4(mat3(camera_data.view)); 
    vec4 clip = camera_data.projection * view_rotation * vec4(aPos, 1.0);
    gl_Position = clip.xyww;
}