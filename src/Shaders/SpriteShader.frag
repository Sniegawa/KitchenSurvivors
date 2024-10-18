#version 460 core

layout (location = 0) out vec2 FragPos;
layout (location = 1) out vec2 Normal;
layout (location = 2) out vec3 Albedo;

in vec2 UV;
in vec2 normal;
in vec2 fragPos;

uniform sampler2D image;
uniform vec3 spriteColor;


void main() 
{

	vec4 diffuse = vec4(spriteColor,1.0) * texture(image,UV);
	
	if(diffuse.w <= 0.05)
		discard;
	Albedo = diffuse.xyz;
	Normal = normal;
	FragPos = fragPos;

}
