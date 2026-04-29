#version 450 core

struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    sampler2D normal;

    float ambient;
    float shininess;
    bool  gamma;
};

struct MaterialSettings
{
    bool useDiffuseTexture;
    bool useSpecularTexture;
    bool useNormalsTexture;
};

in VS_OUT
{
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoord;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;

uniform Material material;
uniform MaterialSettings materialSettings;
uniform vec3 camera_position;

out vec4 FragColor;

#include "Common_Global.glsl"

// Helpers

vec3 get_normal()
{
    if (materialSettings.useNormalsTexture)
    {
        vec3 n = texture(material.normal, fs_in.TexCoord).rgb;
        return normalize(n * 2.0 - 1.0); // tangent space [-1, 1]
    }
    return normalize(fs_in.Normal);
}

vec3 get_light_dir(vec3 light_pos)
{
    if (materialSettings.useNormalsTexture)
        return normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
    return normalize(light_pos - fs_in.FragPos);
}

vec3 get_view_dir()
{
    if (materialSettings.useNormalsTexture)
        return normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    return normalize(camera_position - fs_in.FragPos);
}

vec3 calc_ambient(vec3 light_ambient)
{
    if (materialSettings.useDiffuseTexture)
        return light_ambient * texture(material.diffuse, fs_in.TexCoord).rgb;
    return light_ambient * vec3(material.ambient);
}

vec3 calc_diffuse(vec3 light_diffuse, vec3 lightDir, vec3 normal)
{
    float diff = max(dot(lightDir, normal), 0.0);
    if (materialSettings.useDiffuseTexture)
        return light_diffuse * diff * texture(material.diffuse, fs_in.TexCoord).rgb;
    return light_diffuse * diff * vec3(0.7);
}

vec3 calc_specular(vec3 light_specular, vec3 lightDir, vec3 normal, vec3 viewDir)
{
    vec3  halfwayDir = normalize(lightDir + viewDir);
    float spec       = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    if (materialSettings.useSpecularTexture)
        return light_specular * spec * texture(material.specular, fs_in.TexCoord).rgb;
    return light_specular * spec * vec3(0.7);
}

float calc_attenuation(float dist)
{
    float c = light_data.ambient.a;   // constant
    float l = light_data.diffuse.a;   // linear
    float q = light_data.specular.a;  // quadratic
    return 1.0 / (c + l * dist + q * (dist * dist));
}

// Light type calculators

vec3 calc_directional_light()
{
    // Direction is flipped - directional light points towards the scene
    vec3 lightDir = normalize(-light_data.direction.rgb);
    vec3 normal   = get_normal();
    vec3 viewDir  = get_view_dir();

    vec3 ambient  = calc_ambient(light_data.ambient.rgb);
    vec3 diffuse  = calc_diffuse(light_data.diffuse.rgb, lightDir, normal);
    vec3 specular = calc_specular(light_data.specular.rgb, lightDir, normal, viewDir);

    // No attenuation — directional lights are infinitely distant
    return (ambient + diffuse + specular) * light_data.intensity.a;
}

vec3 calc_point_light()
{
    vec3 lightDir = get_light_dir(light_data.position.rgb);
    vec3 normal   = get_normal();
    vec3 viewDir  = get_view_dir();
    
    vec3 ambient  = calc_ambient(light_data.ambient.rgb);
    vec3 diffuse  = calc_diffuse(light_data.diffuse.rgb, lightDir, normal);
    vec3 specular = calc_specular(light_data.specular.rgb, lightDir, normal, viewDir);

    float dist        = length(light_data.position.rgb - fs_in.FragPos);
    float attenuation = calc_attenuation(dist);

    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular) * light_data.intensity.a;
}

vec3 calc_spot_light()
{
    vec3 lightDir = get_light_dir(light_data.position.rgb);
    vec3 normal   = get_normal();
    vec3 viewDir  = get_view_dir();

    vec3 ambient  = calc_ambient(light_data.ambient.rgb);
    vec3 diffuse  = calc_diffuse(light_data.diffuse.rgb, lightDir, normal);
    vec3 specular = calc_specular(light_data.specular.rgb, lightDir, normal, viewDir);

    // Spotlight cone attenuation (soft edges)
    // cut_off and outer_cut_off are stored as cosines, inner > outer
    float theta          = dot(lightDir, normalize(-light_data.direction.rgb));
    float cut_off        = light_data.position.a;
    float outer_cut_off  = light_data.direction.a;
    float epsilon        = cut_off - outer_cut_off;
    float spot_intensity = clamp((theta - outer_cut_off) / epsilon, 0.0, 1.0);

    diffuse  *= spot_intensity;
    specular *= spot_intensity;

    // Distance attenuation
    float dist        = length(light_data.position.rgb - fs_in.FragPos);
    float attenuation = calc_attenuation(dist);

    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular) * light_data.intensity.a;
}

void main()
{
    vec3 result = vec3(0.0);

    switch (light_data.type)
    {
        case LIGHT_TYPE_DIRECTIONAL: result = calc_directional_light(); break;
        case LIGHT_TYPE_POINT:       result = calc_point_light();       break;
        case LIGHT_TYPE_SPOT:        result = calc_spot_light();        break;
        case LIGHT_TYPE_AREA:        result = calc_spot_light();        break;
        default:                     result = vec3(1.0, 0.0, 0.0);      break;
    }

    if (material.gamma)
        result = pow(result, vec3(1.0 / 2.2));

    FragColor = vec4(result, 1.0);
}