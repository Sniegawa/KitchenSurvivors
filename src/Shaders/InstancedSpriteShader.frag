#version 460 core

layout (location = 0) out vec2 gPosition; 
layout (location = 1) out vec2 gNormal;   
layout (location = 2) out vec3 gAlbedo;  

// Input from vertex shader
in vec2 TexCoord;
in vec2 fragPos;

// Uniform for the texture
uniform sampler2D image;

out vec4 FragColor;

void main() {
    FragColor = texture(image, TexCoord);
	gPosition = fragPos;
	gNormal = vec3(1,0,0).xy;
	gAlbedo = FragColor.xyz;
	if(FragColor.a < 0.5)
	{
		discard;
	}
}