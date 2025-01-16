#version 330 core

in vec2 TexCoords;
out vec4 color;

uniform sampler2D image;
uniform vec3 spriteColor;

void main() 
{
	vec4 tex =texture(image,TexCoords);
	if (tex.a > 0.2)
	{
		color = vec4(spriteColor,1.0) * tex;
	}
	else
	{
		color = vec4(0.0f);
	}
}