#version 460 core

layout(location = 0) in vec4 vertex;

out vec2 UV;
out vec4 FragPos;

uniform mat4 model;
uniform mat4 InverseModel;
uniform mat4 projection;

struct Light
{
	vec4 coords; // vec2 pos, float radius, float ??
	vec4 color; // vec3 color, float intensity
};



void main() {
	UV = vertex.zw;
	FragPos =  model * vec4(vertex.xy,0.0,1.0);
	gl_Position = projection * model * vec4(vertex.xy,0.0,1.0);


}