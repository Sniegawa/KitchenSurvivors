#version 460 core

layout(location = 0) in vec4 aVertex; //xy - position , zw - uv
layout(location = 1) in vec2 aNormal;

out vec2 UV; 
out vec3 normal;
out vec2 fragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 InverseModel;
uniform mat4 projection;

void main() {
	vec4 pos = projection * view * model * vec4(aVertex.xy,0.0,1.0);
	gl_Position =  pos;
	normal = vec3(aNormal,-1);
	UV = aVertex.zw;
	fragPos =  (model * vec4(aVertex.xy,0.0,1.0)).xy;
}