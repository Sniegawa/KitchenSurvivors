#version 460 core

out vec4 FragColor;

in vec2 UV;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D Lightmap;

uniform vec2 ScreenSize;
uniform vec2 PlayerPosition;
uniform int pixelSize;
uniform int LightPixelize;
uniform int RenderMode;
void main()
{
	vec2 PixelizedUV =  floor(UV * ScreenSize / pixelSize) / ScreenSize * pixelSize;
	vec2 FragPos = texture(gPosition, UV).xy;
    vec2 Normal = normalize(texture(gNormal, UV).xy);
    vec3 Albedo = texture(gAlbedo, UV).rgb;
	vec3 PixelizedAlbedo = texture(gAlbedo,PixelizedUV).rgb;
	vec2 PixelatedLightUV = floor(UV * ScreenSize / LightPixelize) / ScreenSize * LightPixelize;

	vec3 LightmapSampled = texture(Lightmap,PixelatedLightUV).rgb;
	if(RenderMode == 2)
	{
		FragColor = vec4(LightmapSampled,1.0);
	}
	else if(RenderMode == 1)
	{
		FragColor = vec4(PixelizedAlbedo,1.0);
	}
	else if (RenderMode == 3)
	{
		FragColor = vec4(LightmapSampled + 0.25 * Albedo,1.0);
	}
	else if (RenderMode == 4)
	{
		FragColor = vec4(LightmapSampled + 0.25 * PixelizedAlbedo,1.0);
	}
	else {
	FragColor = vec4(Albedo,1.0);
	}
}

//Fragment position isn't correct