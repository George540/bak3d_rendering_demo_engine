#version 330 core

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D textureSampler;
uniform vec3 lightPos = vec3(0.0f, 5.0f, 0.0f);
uniform vec3 viewPos;

out vec4 FragColor;

void main()
{
	vec4 textureColor = texture(textureSampler, TexCoord);

	float ambientStrength = 0.5;
	vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
	vec3 ambient = ambientStrength * lightColor;

	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	float specularStrength = 0.3;
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 256);
	vec3 specular = specularStrength * spec * lightColor;

	vec3 result = (ambient + diffuse + specular) * vec3(textureColor.r, textureColor.g, textureColor.b);

	FragColor = vec4(result, 1.0);
}