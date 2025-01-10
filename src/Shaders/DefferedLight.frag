#version 460 core

struct pointLight
{
	vec4 position; // vec2 pos, float att1, float att2
	vec4 color; // vec3 color, float intensity
};



layout(std430, binding = 1) buffer PointLights
{
	pointLight pointlights[];
};


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

const int MaxLightDist = 1000;

void main()
{
	vec2 PixelizedUV =  floor(UV * ScreenSize / pixelSize) / ScreenSize * pixelSize;
	vec2 FragPos = texture(gPosition, UV).xy;
    vec3 Normal = texture(gNormal, UV).xyz;
	Normal = normalize(Normal * 2.0 - 1.0);
    vec3 Albedo = texture(gAlbedo, UV).rgb;
	vec3 LightmapSampled = texture(Lightmap,UV).rgb;
	vec3 LightColor;
	/*for(int i = 0; i < pointlights.length(); i++)
    {
        float dist = distance((FragPos + PlayerPosition),pointlights[i].position.xy);
        if (dist >= MaxLightDist) continue;
        float att = pointlights[i].color.a / (1.0 + pointlights[i].position.z * dist + pointlights[i].position.a * dist * dist);

        LightColor += pointlights[i].color.xyz * att;
    }*/

	/*for(int i = 0; i < dirLights.length(); i++)
	{
		vec3 invLightDir = vec3(pointlights[i].position.xy,1) - vec3(FragPos,0.0);
		//float lightcos = dot(vec3(invLightDir,1.0),Normal);
		float costheta = clamp(dot(Normal,invLightDir)/(length(Normal)*length(invLightDir)),0,1);

	}*/


	if(RenderMode == 1)
	{
		FragColor = vec4(Albedo,1.0);
	}
	else if (RenderMode == 2)
	{
		FragColor = vec4(LightColor + 0.25 * Albedo,1.0);
	}
	else if (RenderMode == 3)
	{
		FragColor = vec4(Albedo,1.0);
	}
	else if (RenderMode == 4)
	{
		FragColor = vec4(Normal,1.0);
	}
	else if(RenderMode == 5)
	{
		FragColor = vec4(LightmapSampled,1.0);
	}
	else {
	FragColor = vec4(Albedo,1.0);
	}
}

//Fragment position isn't correct