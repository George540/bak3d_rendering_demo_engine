#version 450 core

// Particle quad vertex attributes
layout (location = 0) in vec4 vertex; // <vec2 position, vec2 texCoords>

// Particle instance attributes
layout (location = 1) in vec3 instancePosition;
layout (location = 2) in float instanceRotation;
layout (location = 3) in vec4 instanceColor;
layout (location = 4) in float instanceScale;

out vec2 TexCoords;
out vec4 ParticleColor;

#include "Common.glsl"

void main()
{
    // Apply billboarding to instance quad
    vec3 worldPosition = apply_billboarding(
        instancePosition,
        vertex.xy,
        instanceRotation,
        instanceScale,
        view
    );

    // Set texture coordinates and particle color
    TexCoords = vertex.zw;
    ParticleColor = instanceColor;

    // Apply the projection and view matrices
    gl_Position = projection * view * vec4(worldPosition, 1.0);
}