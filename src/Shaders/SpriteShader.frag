#version 460 core

in vec2 UV;
in vec4 FragPos;
out vec4 color;

uniform sampler2D image;
uniform vec3 spriteColor;

struct Light
{
	vec4 coords; // vec2 pos, float att1, float att2
	vec4 color; // vec3 color, float ??
};


layout(std430, binding = 1) buffer SceneLights
{
	Light lights[];
};

uniform vec2 PlayerPos;

void main() 
{
	vec4 diffuse = vec4(spriteColor,1.0) * texture(image,UV);

	vec3 lighting;// = vec3(0.2f); //Ambient
	
	for(int i = 0; i < lights.length(); i++)
	{
		Light light = lights[i];
		float dist = length((light.coords.xy-PlayerPos)-FragPos.xy);

		float att = clamp(1.0 / (1.0 + light.coords.z * dist + light.coords.w * dist * dist),0.0,1.0);
		
		color += vec4(att * (diffuse.xyz + vec3(0.2) + light.color.xyz),diffuse.w);

		
	}
	

}