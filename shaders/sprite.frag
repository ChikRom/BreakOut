#version 460 core

in vec2 TexCoords;

out vec4 Colour;

uniform sampler2D image;
uniform vec3 spriteColour;

void main()
{
	Colour = vec4(spriteColour,1.0) * texture(image,TexCoords);
}