#version 460 core

layout (location = 0) in vec4 vertex;

out vec2 TexCoords;
out vec4 ParticleColour;

uniform mat4 projection;
uniform vec2 offset;
uniform vec4 colour;

void main()
{
	float scale = 20.0f;
	TexCoords = vertex.zw;
	ParticleColour = colour;
	gl_Position = projection * vec4((vertex.xy * scale) + offset, 0.0f, 1.0f);
}