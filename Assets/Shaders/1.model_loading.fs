#version 330 core

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float ambient;
    vec3 normal;
    vec3 roughness;
    float shininess;
    bool useDiffuseTexture;
};

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

out vec4 FragColor;

in VS_OUT
{
    vec2 TexCoord;
    vec3 Normal;
    vec3 FragPos;
} fs_in;

uniform sampler2D texture_diffuse1;
uniform vec3 viewPos;
uniform Material material;
uniform Light light;
uniform bool gamma;

void main()
{    
    vec4 textureColor = texture(texture_diffuse1, fs_in.TexCoord);

	// ambient
    vec3 ambient = vec3(1.0);
    if (material.useDiffuseTexture)
    {
        ambient = light.ambient * vec3(texture(material.diffuse, fs_in.TexCoord));
    }
    else
    {
        ambient *= light.ambient * vec3(material.ambient);
    }

	// diffuse 
    vec3 norm = normalize(fs_in.Normal);
    vec3 lightDir = normalize(light.position - fs_in.FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = vec3(1.0);
    if (material.useDiffuseTexture)
    {
        diffuse = light.diffuse * diff * vec3(texture(material.diffuse, fs_in.TexCoord));
    }
    else
    {
        diffuse = light.diffuse * diff * vec3(0.7);
    }

	// specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular, fs_in.TexCoord));  

    float distance = length(light.position - fs_in.FragPos);
    float attenuation = 1.0 / (gamma ? distance * distance : distance);

    if (gamma)
    {
        diffuse *= attenuation;
        specular *= attenuation;
    }

	vec3 result = ambient + diffuse + specular;

    // Process gamma correction if enabled
    if (gamma)
    {
        result = pow(result, vec3(1.0/2.2));
    }

	FragColor = vec4(result, 1.0);
}