#version 330 core

layout (location = 0) in vec2 position;
uniform vec2 screenSize;

void main()
{
	vec2 ndcPos = vec2
	(
        2.0 * position.x / screenSize.x - 1.0,
        1.0 - 2.0 * position.y / screenSize.y
	);

	gl_Position = vec4(ndcPos, 0.0, 1.0);
}