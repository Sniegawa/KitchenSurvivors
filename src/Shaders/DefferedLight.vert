#version 330 core
layout (location = 0) in vec4 Position;

out vec2 UV;

void main()
{
    // Pass-through,we render textures on full screen quad
    gl_Position = vec4(Position.xy, 0.0, 1.0); 
    UV = Position.zw;
}