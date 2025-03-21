#version 330 core

layout(location = 0) in vec2 aPos;

uniform vec2 center;
uniform mat4 projection;
uniform mat4 model;

void main() {
	vec2 worldPos = aPos + center;
	gl_Position = projection * model * vec4(worldPos,0.0,1.0);
}