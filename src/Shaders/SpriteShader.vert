#version 460 core

layout(location = 0) in vec4 aVertex; //xy - position , zw - uv
layout(location = 1) in vec2 aNormal;

out vec2 UV; 
out vec2 normal;
out vec2 fragPos;

uniform mat4 model;
uniform mat4 InverseModel;
uniform mat4 projection;

struct Light
{
	vec4 coords; // vec2 pos, float radius, float ??
	vec4 color; // vec3 color, float intensity
};



void main() {
	gl_Position = projection * model * vec4(aVertex.xy,0.0,1.0);
	normal = aNormal;
	UV = aVertex.zw;
	fragPos = aVertex.xy;
}