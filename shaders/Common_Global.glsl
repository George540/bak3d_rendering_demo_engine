#define LIGHT_TYPE_DIRECTIONAL 0
#define LIGHT_TYPE_POINT       1
#define LIGHT_TYPE_SPOT        2
#define LIGHT_TYPE_AREA        3

#define DEBUG_VIEW_LIT              0
#define DEBUG_VIEW_GBUFFER_POSITION 1
#define DEBUG_VIEW_GBUFFER_ALBEDO   2
#define DEBUG_VIEW_GBUFFER_NORMALS  3
#define DEBUG_VIEW_GBUFFER_SPECULAR 4
#define DEBUG_VIEW_DEPTH            5
#define DEBUG_VIEW_SHADOWMASK       6
#define DEBUG_VIEW_AO               7
#define DEBUG_VIEW_MAX              8

layout (std140, binding = 0) uniform Camera
{
    mat4 projection;
    mat4 view;
    vec4 position; // xyz = camera position, w = free
} camera_data;

uniform mat4 model;

struct ShadowData
{
    mat4 light_space_matrix;
    vec4 bias_params;       // x = const bias,      y = slope bias,     z = normal offset (shadow texels), w = PCF radius
    vec4 projection_params; // x = near,            y = far,            z = frustum scale,                 w = 1 perspective / 0 ortho
    vec4 map_params;        // x = resolution,      y = enabled,        z = light size,                    w = max filter radius (texels)
    vec4 pcss_params;       // x = blocker samples, y = filter samples, z = flags,                         w = unused
};

layout (std430, binding = 14) readonly buffer ShadowBuffer
{
    ShadowData entries[];
} shadow_data;

layout (binding = 8) uniform sampler2DArray shadow_map;

struct LightData
{
    vec4 position;  // .rgb = position,  .a = cut_off
    vec4 direction; // .rgb = direction, .a = outer_cut_off
    vec4 ambient;   // .rgb = ambient,   .a = radius
    vec4 diffuse;   // .rgb = diffuse,   .a = intensity
    vec4 specular;  // .rgb = specular,  .a = PADDING
    int type;       // Light caster type
    int shadow_index; // -1 = no shadow, 0 and greater = shadows
    float padding[2];
};

layout (std430, binding = 15) buffer LightDataBuffer
{
    LightData lights[];
} light_buffer;

uniform int active_light_count;

layout (std140, binding = 2) uniform PageData
{
    vec4 depth_settings; // .r = near, .g = far, .gb = currently unused
    int debug_mode;      // 0 = color, 1 = depth, TBD: 2 = AO, 3 = normals, etc
    float padding[3];
} page_data;

// Nathan Reed, "Hash Function for GPU Rendering", 2021, https://www.reedbeta.com/blog/hash-functions-for-gpu-rendering/
uint pcg_hash(uint input_value)
{
    uint state = input_value * 747796405u + 2891336453u;
    uint word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
    return (word >> 22u) ^ word;
}

float pcg_hash_rand01(uint seed)
{
    return float(pcg_hash(seed)) / 4294967295.0; // normalize to 0.0-1.0
}

vec3 apply_billboarding(
        vec3 world_position,
        vec2 vertex_xy,
        float rotation,
        float scale,
        float scale_multiplier,
        mat4 view)
{
    // Extract camera alignment vectors from the view matrix
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

    // If scale is 0.0, calculate a dynamic scale based on distance to camera
    float final_scale = scale;
    if (scale == 0.0 && scale_multiplier > 0.0f)
    {
        // Transform the object's world position into view space to get its depth (0-2)
        // that represents the camera forward vector, while view[3][2] is the translation offset.
        float depth = -(view[0][2] * world_position.x +
                        view[1][2] * world_position.y +
                        view[2][2] * world_position.z +
                        view[3][2]);

        // Use depth as the baseline scale factor.
        final_scale = depth * scale_multiplier;
    }

    return world_position + (camera_right * rotated_vertex.x + camera_up * rotated_vertex.y) * final_scale;
}


float get_light_distance(LightData light, vec3 frag_position)
{
    return length(light.position.xyz - frag_position);
}

vec3 get_light_direction(LightData light, vec3 frag_position)
{
    return normalize(light.position.xyz - frag_position);
}

vec3 get_view_direction(vec3 frag_position)
{
    return normalize(camera_data.position.xyz - frag_position);
}

float get_linear_depth(vec3 frag_position)
{
    return distance(frag_position, camera_data.position.xyz);
}

float calculate_wboit_weight(float depth, float alpha)
{
    float weight = alpha * max(1e-2, min(3e3, 0.03 / (1e-5 + pow(depth / 5.0, 4.0))));
    return weight;
}

bool get_point_sphere_normal(vec2 point_coord, mat4 view, out vec3 world_normal)
{
    vec2 circle_coord = point_coord * 2.0 - 1.0;

    float r2 = dot(circle_coord, circle_coord);
    if (r2 > 1.0)
    {
        world_normal = vec3(0.0);
        return false;
    }

    // Sphere equation based on z:
    // x^2 + y^2 + z^2 = 1 -> z = sqrt(1.0 - x^2 - y^2)
    float z = sqrt(1.0 - r2);

    // Camera basis vectors in world space (rows of the view matrix)
    vec3 camera_right = vec3(view[0][0], view[1][0], view[2][0]);
    vec3 camera_up = vec3(view[0][1], view[1][1], view[2][1]);
    vec3 camera_forward = vec3(view[0][2], view[1][2], view[2][2]); // points away from camera into scene

    // Flip forward so normal points toward the camera
    world_normal = normalize(circle_coord.x * camera_right + circle_coord.y * camera_up - z * camera_forward);
    return true;
}

// Smooth radial alpha falloff from opaque center to transparent edges (like default_particle.png)
// Higher softness = softer edges, lower softness = closer to a hard circle
float calculate_point_alpha_falloff(vec2 point_coord, float softness)
{
    vec2 circle_coord = point_coord * 2.0 - 1.0;
    float radius = length(circle_coord);
    return 1.0 - smoothstep(1.0 - softness, 1.0, radius);
}

float process_attenuation(LightData light, vec3 frag_position)
{
    float distance = get_light_distance(light, frag_position);
    float radius = light.ambient.a;
    float c = 1.0;
    float l = 4.5 / radius;
    float q = 75.0 / (radius * radius);
    return 1.0 / (c + l * distance + q * (distance * distance));
}

// Enhanced translucency handling peak alignment and side profile lighting
float calculate_translucency(vec3 view_direction, vec3 light_direction, float scatter_power)
{
    float alignment = dot(view_direction, light_direction);

    // Narrow peaks: Handles back-lit and front-lit edge highlights
    float back_scatter = max(-alignment, 0.0);
    float front_scatter = max(alignment, 0.0);
    float directional_scatter = pow(back_scatter + front_scatter, scatter_power);

    // Isotropic base: Simulates internally bounced light that escapes through the sides
    // When alignment is 0.0 (exact side view), this provides a solid 0.5 baseline contribution
    float isotropic_scatter = alignment * 0.5 + 0.5;

    // Linearly blend the two phases
    // 0.2 gives a subtle volumetric density look from the side without flattening your highlights
    return mix(isotropic_scatter, directional_scatter, 0.8);
}

float shadow_linear_depth(vec4 projection_params, float depth)
{
    float near = projection_params.x;
    float far = projection_params.y;

    if (projection_params.w > 0.5) // perspective
    {
        float ndc = depth * 2.0 - 1.0;
        return (2.0 * near * far) / (far + near - ndc * (far - near));
    }

    return near + depth * (far - near); // ortho
}

// Golden-angle spiral: even radial distribution for any sample count
vec2 vogel_disk(int index, int count, float rotation)
{
    float radius = sqrt((float(index) + 0.5) / float(count));
    float angle = float(index) * 2.39996323 + rotation;
    return radius * vec2(cos(angle), sin(angle));
}

// World-position hash: per-pixel rotation without gl_FragCoord (this file is also included by non-fragment stages)
float shadow_disk_rotation(vec3 p)
{
    uint seed = pcg_hash(floatBitsToUint(p.x) ^ pcg_hash(floatBitsToUint(p.y) ^ pcg_hash(floatBitsToUint(p.z))));
    return pcg_hash_rand01(seed) * 6.28318530718;
}

// textureLod: implicit derivatives are undefined inside non-uniform branches
float shadow_sample_linear_depth(vec4 projection_params, vec2 uv, float layer)
{
    return shadow_linear_depth(projection_params, textureLod(shadow_map, vec3(uv, layer), 0.0).r);
}

// Returns 1.0 = fully lit, 0.0 = fully shadowed. light_direction points from the fragment to the light.
float calculate_shadow(LightData light, vec3 frag_position, vec3 normal, vec3 light_direction)
{
    if (light.shadow_index < 0)
    {
        return 1.0;
    }

    // Surface faces away from the light: diffuse/specular are zero anyway
    float n_dot_l = min(dot(normal, light_direction), 1.0);
    if (n_dot_l <= 0.0)
    {
        return 0.0;
    }

    ShadowData entry = shadow_data.entries[light.shadow_index];
    bool is_perspective = entry.projection_params.w > 0.5;
    float near_plane = entry.projection_params.x;
    float far_plane = entry.projection_params.y;
    float frustum_scale = entry.projection_params.z;
    float map_size = entry.map_params.x;
    float texel_uv = 1.0 / map_size;
    uint flags = uint(entry.pcss_params.z);

    // Early out: behind or beyond a perspective light
    vec4 probe = entry.light_space_matrix * vec4(frag_position, 1.0);
    if (is_perspective && (probe.w <= 0.0 || probe.w >= far_plane))
    {
        return 1.0;
    }

    // World-space size of one shadow texel at this fragment
    float light_distance = is_perspective ? probe.w : 1.0;
    float texel_world_size = 2.0 * frustum_scale * light_distance / map_size;

    // Normal offset
    float sin_theta = sqrt(1.0 - n_dot_l * n_dot_l);
    vec3 offset_position = frag_position + normal * (entry.bias_params.z * texel_world_size * sin_theta);

    vec4 light_clip = entry.light_space_matrix * vec4(offset_position, 1.0);
    if (light_clip.w <= 0.0)
    {
        return 1.0;
    }
    vec3 coords = light_clip.xyz / light_clip.w * 0.5 + 0.5;

    // Early out: outside the light's frustum
    if (coords.z > 1.0 || coords.z < 0.0
        || any(lessThan(coords.xy, vec2(0.0))) || any(greaterThan(coords.xy, vec2(1.0))))
    {
        return 1.0;
    }

    float receiver_depth = shadow_linear_depth(entry.projection_params, coords.z);
    float world_per_uv = 2.0 * (is_perspective ? frustum_scale * receiver_depth : frustum_scale);
    float min_radius_uv = entry.bias_params.w * texel_uv;
    float max_radius_uv = max(entry.map_params.w * texel_uv, min_radius_uv);
    float light_size = entry.map_params.z;

    // Bias: receiver plane (analytic, from the normal) or constant + slope
    float bias = entry.bias_params.x * texel_world_size;
    vec2 plane_slope = vec2(0.0); // linear depth change per unit UV offset
    float plane_clamp = 0.0;
    if ((flags & 2u) != 0u)
    {
        mat4 m = entry.light_space_matrix;
        vec3 axis_right = normalize(vec3(m[0][0], m[1][0], m[2][0]));
        vec3 axis_up = normalize(vec3(m[0][1], m[1][1], m[2][1]));
        plane_slope = vec2(dot(normal, axis_right), dot(normal, axis_up)) * world_per_uv / max(n_dot_l, 0.1);
        plane_clamp = 2.0 * max_radius_uv * world_per_uv;
    }
    else
    {
        bias += entry.bias_params.y * (1.0 - n_dot_l) * (1.0 + entry.bias_params.w) * texel_world_size;
    }
    float biased_depth = receiver_depth - bias;

    float layer = float(light.shadow_index);
    float rotation = shadow_disk_rotation(frag_position);
    float filter_radius_uv = min_radius_uv;

    // PCSS blocker search (also serves as the coarse early-out test)
    if ((flags & 1u) != 0u)
    {
        float search_world = is_perspective
        ? light_size * (receiver_depth - near_plane) / receiver_depth
        : light_size * (receiver_depth - near_plane);
        float search_radius_uv = clamp(search_world / world_per_uv, min_radius_uv, max_radius_uv);

        int blocker_samples = int(entry.pcss_params.x);
        int blocker_count = 0;
        float blocker_sum = 0.0;
        for (int i = 0; i < blocker_samples; ++i)
        {
            vec2 offset = vogel_disk(i, blocker_samples, rotation) * search_radius_uv;
            float blocker_depth = shadow_sample_linear_depth(entry.projection_params, coords.xy + offset, layer);
            float sample_receiver = biased_depth + clamp(dot(plane_slope, offset), -plane_clamp, plane_clamp);
            if (blocker_depth < sample_receiver)
            {
                blocker_count++;
                blocker_sum += blocker_depth;
            }
        }

        if (blocker_count == 0)
        {
            return 1.0; // nothing in the region: fully lit
        }

        float average_blocker = blocker_sum / float(blocker_count);
        float penumbra_world = is_perspective
        ? light_size * (receiver_depth - average_blocker) / average_blocker
        : light_size * (receiver_depth - average_blocker);
        filter_radius_uv = clamp(penumbra_world / world_per_uv, min_radius_uv, max_radius_uv);

        // Everything blocked and the filter fits inside the search region: umbra
        if (blocker_count == blocker_samples && filter_radius_uv <= search_radius_uv)
        {
            return 0.0;
        }
    }

    // Filter (penumbra pixels only reach here under PCSS)
    int filter_samples = filter_radius_uv < 0.5 * texel_uv ? 1 : int(entry.pcss_params.y);
    float lit = 0.0;
    for (int i = 0; i < filter_samples; ++i)
    {
        vec2 offset = vogel_disk(i, filter_samples, rotation) * filter_radius_uv;
        float stored_depth = shadow_sample_linear_depth(entry.projection_params, coords.xy + offset, layer);
        float sample_receiver = biased_depth + clamp(dot(plane_slope, offset), -plane_clamp, plane_clamp);
        lit += sample_receiver <= stored_depth ? 1.0 : 0.0;
    }
    return lit / float(filter_samples);
}

vec3 process_deferred_light_contribution(
        LightData light,
        vec3 frag_pos,
        vec3 normal,
        vec3 albedo,
        vec3 ambient_basis,
        float specular_strength,
        float shininess,
        vec3 view_direction)
{
    vec3 light_direction;
    float attenuation = 1.0;
    float spot_intensity = 1.0;

    if (light.type == LIGHT_TYPE_DIRECTIONAL)
    {
        light_direction = normalize(-light.direction.xyz);
    }
    else
    {
        light_direction = normalize(light.position.xyz - frag_pos);
        float light_distance = length(light.position.xyz - frag_pos);
        float radius = light.ambient.a;
        // Attenuation handling
        float c = 1.0;
        float l = 4.5 / radius;
        float q = 75.0 / (radius * radius);
        attenuation = 1.0 / (c + l * light_distance + q * (light_distance * light_distance));


        if (light.type == LIGHT_TYPE_SPOT || light.type == LIGHT_TYPE_AREA)
        {
            vec3 spot_direction = normalize(light.direction.xyz);
            float theta = dot(light_direction, -spot_direction);
            float cut_off = light.position.a;
            float outer_cut_off = light.direction.a;
            float epsilon = cut_off - outer_cut_off;
            spot_intensity = clamp((theta - outer_cut_off) / max(epsilon, 0.001), 0.0, 1.0);
        }
    }

    float diff = max(dot(normal, light_direction), 0.0);
    float ambient_face = clamp(dot(normal, light_direction) * 0.5 + 0.5, 0.0, 1.0);
    
    vec3 halfway_dir = normalize(light_direction + view_direction);
    float spec = pow(max(dot(normal, halfway_dir), 0.0), max(shininess, 1.0));

    vec3 ambient_term = light.ambient.rgb * ambient_basis * ambient_face;
    vec3 diffuse_term = light.diffuse.rgb * diff * albedo;
    vec3 specular_term = light.specular.rgb * spec * specular_strength;

    // Shadow term multipliers
    float shadow = calculate_shadow(light, frag_pos, normal, light_direction);
    diffuse_term *= shadow;
    specular_term *= shadow;

    // spot cone, then distance attenuation, applied per-term
    ambient_term *= spot_intensity;
    diffuse_term *= spot_intensity;
    specular_term *= spot_intensity;
    ambient_term *= attenuation;
    diffuse_term *= attenuation;
    specular_term *= attenuation;

    return (ambient_term + diffuse_term + specular_term) * light.diffuse.a;
}

vec3 process_particle_light_forward_translucency(LightData light, vec3 frag_position, vec3 view_direction, float scatter_power)
{
    vec3 light_direction;
    float attenuation = 1.0;
    float light_intensity = 1.0;

    if (light.type == LIGHT_TYPE_DIRECTIONAL)
    {
        light_direction = normalize(-light.direction.xyz);
    }
    else
    {
        // Compute direction from the specific light's position
        light_direction = normalize(light.position.xyz - frag_position);

        // Pass the light's explicit attenuation radius (.a channel of ambient)
        attenuation = process_attenuation(light, frag_position);

        if (light.type == LIGHT_TYPE_SPOT || light.type == LIGHT_TYPE_AREA)
        {
            vec3 world_spotlight_direction = normalize(light.direction.xyz);
            float theta = dot(light_direction, -world_spotlight_direction);
            float cut_off = light.position.a;
            float outer_cut_off = light.direction.a;
            float epsilon = cut_off - outer_cut_off;
            light_intensity = clamp((theta - outer_cut_off) / max(epsilon, 0.001), 0.0, 1.0);
        }
    }

    // Directional translucent scattering
    float translucency = calculate_translucency(view_direction, light_direction, scatter_power);

    // Add an isotropic/wrapped diffuse component for side angles (90 degrees)
    float side_scattering = clamp(dot(light_direction, view_direction) * 0.5 + 0.5, 0.0, 1.0);

    // Smoothly blend directional translucency with all-around side scattering
    float final_scattering = max(translucency, side_scattering * 0.25);

    vec3 ambient = light.ambient.rgb;
    vec3 scattered = light.diffuse.rgb * final_scattering;

    // Accumulate all factors alongside the light intensity (.a channel of diffuse)
    return (ambient + scattered) * attenuation * light_intensity * light.diffuse.a;
}

vec3 calculate_total_particle_lighting(vec3 frag_position, vec3 view_direction, float scatter_power)
{
    vec3 total_scattered_light = vec3(0.0);

    // Dynamic iteration over the unsized SSBO array up to the active light count uniform
    for (int i = 0; i < active_light_count; ++i)
    {
        // Extract the current light payload chunk locally
        LightData light = light_buffer.lights[i];

        // Accumulate this specific light's translucent volumetric contribution
        total_scattered_light += process_particle_light_forward_translucency(light, frag_position, view_direction, scatter_power);
    }

    return total_scattered_light;
}
