#version 330 core

layout (location = 0) in vec4 vertex; // <vec2 position, vec2 texCoords>

out vec2 TexCoords;
out vec4 ParticleColor;

uniform mat4 projection;
uniform mat4 view;
uniform vec3 position;
uniform float rotation;
uniform float scale;
uniform vec4 color;

void main()
{
    // Extract the camera's right and up vectors from the view matrix
    vec3 cameraRight = vec3(view[0][0], view[1][0], view[2][0]);
    vec3 cameraUp = vec3(view[0][1], view[1][1], view[2][1]);

    // Adjust the vertex positions to be centered around (0, 0)
    vec2 centeredVertex = vertex.xy - vec2(0.5, 0.5);

    // Convert rotation angle from degrees to radians
    float rotationRadians = radians(rotation);

    // Apply rotation using cos and sin from transformation matrix
    float cosTheta = cos(rotationRadians);
    float sinTheta = sin(rotationRadians);
    vec2 rotatedVertex = vec2(
        cosTheta * centeredVertex.x - sinTheta * centeredVertex.y,
        sinTheta * centeredVertex.x + cosTheta * centeredVertex.y
    );

    // Apply the billboarding technique by modifying the vertex position
    vec3 worldPosition = position + (cameraRight * rotatedVertex.x + cameraUp * rotatedVertex.y) * scale;

    // Set texture coordinates and particle color
    TexCoords = vertex.zw;
    ParticleColor = color;

    // Apply the projection and view matrices
    gl_Position = projection * view * vec4(worldPosition, 1.0);
}