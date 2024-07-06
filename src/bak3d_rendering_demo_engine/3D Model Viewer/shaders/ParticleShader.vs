#version 330 core

// Particle quad vertex attributes
layout (location = 0) in vec4 vertexInput; // <vec2 position (xy), vec2 texCoords (zw)>

// Particle instance attributes
layout (location = 1) in vec4 instanceColor;
layout (location = 2) in mat4 instanceWorldMatrix; // World matrix for the particle instance

out vec2 TexCoords;
out vec4 ParticleColor;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    // Extract the camera's right and up vectors from the view matrix
    vec3 cameraRight = vec3(view[0][0], view[1][0], view[2][0]);
    vec3 cameraUp = vec3(view[0][1], view[1][1], view[2][1]);

    // Adjust the vertex positions to be centered around (0, 0)
    vec2 centeredVertex = vertexInput.xy - vec2(0.5, 0.5);

    // Apply the particle's world matrix transformation to the centered vertex position
    vec4 worldPosition = instanceWorldMatrix * vec4(centeredVertex, 0.0, 1.0);

    // Combine with the camera's right and up vectors to achieve the billboarding effect
    vec3 billboardPosition = vec3(worldPosition.x * cameraRight + worldPosition.y * cameraUp + vec3(instanceWorldMatrix[3]));

    // Set texture coordinates and particle color
    TexCoords = vertexInput.zw;
    ParticleColor = instanceColor;

    // Apply the projection and view matrices
    gl_Position = projection * view * vec4(billboardPosition, 1.0);
}