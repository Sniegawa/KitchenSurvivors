#version 330 core

// Input from vertex shader
in vec2 TexCoord;

// Uniform for the texture
uniform sampler2D image;

out vec4 FragColor;

void main() {
    FragColor = texture(image, TexCoord);
}