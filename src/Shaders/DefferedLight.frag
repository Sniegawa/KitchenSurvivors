#version 460 core

out vec4 FragColor;

in vec2 UV;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;

uniform vec2 ScreenSize;
uniform vec2 PlayerPosition;
uniform int pixelSize;

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
	vec2 PixelatedUV = floor(UV*ScreenSize/pixelSize) / ScreenSize * pixelSize;

    float pixelGridSize = 0.5; // Adjust this to control the grid size in world units
	vec2 PixelatedFragPos = floor(FragPos / pixelGridSize) * pixelGridSize;

	vec2 PixFragPos = texture(gPosition,PixelatedUV).xy;
	//Lighting calculation
	vec3 LightColor;
	for(int i = 0; i < pointlights.length(); i++)
	{
		//vec2 pixelatedLightPos = floor(pointlights[i].position.xy*ScreenSize/pixelSize) / ScreenSize * pixelSize;
		if(distance(PixelatedFragPos,pointlights[i].position.xy-PlayerPosition) < 100){
			LightColor += pointlights[i].color.xyz * 0.5;
		}
	}


	FragColor = vec4(Albedo * (0.25 + LightColor),1.0);
	//FragColor = vec4(PixFragPos,0.0,1.0);
}

//Fragment position isn't correct