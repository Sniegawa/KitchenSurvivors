#version 460 core

out vec4 FragColor;

in vec2 UV;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;

struct pointLight
{
	vec2 position; // vec2 pos, float att1, float att2
	vec3 color; // vec3 color, float ??
};
 
layout(std430, binding = 1) buffer PointLights
{
	pointLight pointlights[];
};

void main()
{

	FragColor = vec4(UV,0.0,1.0);

}