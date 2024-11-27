#version 460 core

out vec4 color;
layout (location = 0) out vec2 gPosition; 
layout (location = 1) out vec2 gNormal;   
layout (location = 2) out vec3 gAlbedo;  

in vec2 UV;
in vec2 normal;
in vec2 fragPos;

uniform sampler2D image;
uniform vec3 spriteColor;


void main() 
{

	vec4 diffuse = vec4(spriteColor,1.0) * texture(image,UV);
	color = diffuse;
	gAlbedo = diffuse.xyz;
	gNormal = vec3(1,0,0).xy;
	gPosition = fragPos;
	if(diffuse.a < 0.5)
	{
		discard;
	}
}
