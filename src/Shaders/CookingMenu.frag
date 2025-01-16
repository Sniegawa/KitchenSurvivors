#version 330 core

out vec4 FragColor;

uniform vec4 uColor; //Circle color

void main()
{
	FragColor = uColor;
}