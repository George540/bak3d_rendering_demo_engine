#define LIGHT_TYPE_DIRECTIONAL 0
#define LIGHT_TYPE_POINT       1
#define LIGHT_TYPE_SPOT        2
#define LIGHT_TYPE_AREA        3

layout (std140, binding = 0) uniform Camera
{
    mat4 projection;
    mat4 view;
} camera_data;

uniform mat4 model;

layout (std140, binding = 1) uniform LightData
{
    vec4 position;  // .rgb = position,  .a = cut_off       (cosine of inner cone angle)
    vec4 direction; // .rgb = direction, .a = outer_cut_off (cosine of outer cone angle)
    vec4 ambient;   // .rgb = ambient,   .a = radius        (attenuation radius)
    vec4 diffuse;   // .rgb = diffuse,   .a = intensity
    vec4 specular;  // .rgb = specular,  .a = PADDING
    int type;
    float padding[3];
} light_data;

vec3 apply_billboarding(
    vec3 world_position,
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