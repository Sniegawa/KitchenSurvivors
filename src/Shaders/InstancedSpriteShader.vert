#version 460 core

// Per-vertex attributes
layout (location = 0) in vec2 aPos;       // 2D vertex position
layout (location = 1) in vec2 aTexCoord;  // Texture coordinates

// Per-instance attribute
layout (location = 2) in mat4 instanceModelMatrix;  // 4x4 model matrix for each instance

uniform mat4 projection;

// Output to fragment shader
out vec2 TexCoord;
out vec2 fragPos;

void main() {
    // Apply the instance-specific model matrix
    vec4 transformedPos = projection * instanceModelMatrix * vec4(aPos, 0.0, 1.0);
    gl_Position = transformedPos;
    
    TexCoord = aTexCoord;  // Pass texture coordinates to the fragment shader
    fragPos = (instanceModelMatrix * vec4(aPos, 0.0, 1.0)).xy;
}