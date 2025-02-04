#version 450 core

struct Material 
{
    sampler2D diffuse;
    sampler2D specular;
    float ambient;
    sampler2D normal;
    vec3 roughness;
    float shininess;
    bool gamma;
};

struct MaterialSettings
{
    bool useDiffuseTexture;
    bool useSpecularTexture;
    bool useNormalsTexture;
};

struct Light 
{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float intensity;
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

uniform vec3 viewPos;
uniform Material material;
uniform MaterialSettings materialSettings;
uniform Light light;

out vec4 FragColor;

void main()
{    
	// ambient
    vec3 ambient = vec3(1.0);
    if (materialSettings.useDiffuseTexture)
    {
        ambient = light.ambient * texture(material.diffuse, fs_in.TexCoord).rgb;
    }
    else
    {
        ambient *= light.ambient * vec3(material.ambient);
    }

    // normal
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightDir = normalize(light.position - fs_in.FragPos);
    if (materialSettings.useNormalsTexture)
    {
        normal = texture(material.normal, fs_in.TexCoord).rgb; // obtain normal from normal map in range [0,1]
        normal = normalize(normal * 2.0 - 1.0);  // this normal is in tangent space, [-1,1]
        lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
    }
    float diff = max(dot(lightDir, normal), 0.0);

    // diffuse 
    vec3 diffuse = vec3(1.0);
    if (materialSettings.useDiffuseTexture)
    {
        diffuse = light.diffuse * diff * texture(material.diffuse, fs_in.TexCoord).rgb;
    }
    else
    {
        diffuse = light.diffuse * diff * vec3(0.7);
    }

	// specular
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    vec3 reflectDir = reflect(-lightDir, normal); 
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    vec3 specular = vec3(1.0);
    if (materialSettings.useSpecularTexture)
    {
        specular = light.specular * spec * texture(material.specular, fs_in.TexCoord).rgb;
    }
    else
    {
        specular = light.specular * spec * 0.7;
    }

    float distance = length(light.position - fs_in.FragPos);
    float attenuation = 1.0 / (material.gamma ? distance * distance : distance);

    if (material.gamma)
    {
        diffuse *= attenuation;
        specular *= attenuation;
    }

	vec3 result = ambient + diffuse + specular;

    // Process gamma correction if enabled
    if (material.gamma)
    {
        result = pow(result, vec3(1.0/2.2));
    }

	FragColor = vec4(result * light.intensity, 1.0);
}