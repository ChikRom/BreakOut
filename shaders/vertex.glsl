#version 460 core
layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inColour;

out vec3 Colour;

void main()
{
	Colour = inColour;
	gl_Position = vec4(inPos,1.0);
}