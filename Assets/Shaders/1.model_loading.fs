#version 330 core

struct Material {
    float diffuse;
    float specular;
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

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D texture_diffuse1;
uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main()
{    
    vec4 textureColor = texture(texture_diffuse1, TexCoord);

	// ambient
    vec3 ambient = light.ambient * vec3(material.ambient);

	// diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * vec3(material.diffuse));

	// specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * vec3(material.specular));  

	vec3 result = ambient + diffuse + specular;
    if (material.useDiffuseTexture)
    {
         result *= vec3(textureColor.r, textureColor.g, textureColor.b);
    }

	FragColor = vec4(result, 1.0);
}