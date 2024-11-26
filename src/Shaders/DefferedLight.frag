#version 460 core

out vec4 FragColor;

in vec2 UV;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;

struct pointLight
{
	vec4 position; // vec2 pos, float att1, float att2
	vec4 color; // vec3 color, float ??
};
 
layout(std430, binding = 1) buffer PointLights
{
	pointLight pointlights[];
};

void main()
{
    vec2 FragPos = texture(gPosition, UV).xy;
    vec2 Normal = normalize(texture(gNormal, UV).xy);
    vec3 Albedo = texture(gAlbedo, UV).rgb;
	FragColor = texture(gNormal,UV);
	FragColor.xyz = Albedo;
}