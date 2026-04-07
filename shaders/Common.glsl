// ============================================================================
// SHADER INCLUDE FILE:
// Includes are not applying the same way in OpenGL shaders as in code.
// When a shader include file is written in a shader,
// the application conncatenates all of its functions on its location.
// ============================================================================

vec3 apply_billboarding
    (vec3 world_position,
    vec2 vertex_xy,
    float rotation,
    float scale,
    mat4 view)
{
    vec3 camera_right = vec3(view[0][0], view[1][0], view[2][0]);
    vec3 camera_up = vec3(view[0][1], view[1][1], view[2][1]);

    vec2 centered_vertex = vertex_xy - vec2(0.5, 0.5);

    float rotation_radians = radians(rotation);
    float cos_theta = cos(rotation_radians);
    float sin_theta = sin(rotation_radians);

    vec2 rotated_vertex = vec2(
        cos_theta * centered_vertex.x - sin_theta * centered_vertex.y,
        sin_theta * centered_vertex.x + cos_theta * centered_vertex.y
    );

    return world_position + (camera_right * rotated_vertex.x + camera_up * rotated_vertex.y) * scale;
}
