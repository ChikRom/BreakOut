#version 460 core
in vec2 TexCoords;
out vec4 colour;

uniform sampler2D	scene;
uniform vec2		offsets[9];
uniform int			edge_kernel[9];
uniform float		blur_kernel[9];

uniform bool chaos;
uniform bool confuse;
uniform bool shake;

void main()
{
	colour = vec4(0.0f);
	vec3 samplers[9];
	if (chaos || shake)
		{
			for (int i = 0; i < 9 ; i++)
				samplers[i] = vec3(texture(scene,TexCoords.st + offsets[i]));
		}
	if (chaos)
	{
		for (int i = 0; i < 9; i++)
			colour += vec4(samplers[i] * float(edge_kernel[i]), 0.0f);
		colour.a = 1.0f;
	}
	else if (confuse)
	{
		colour = vec4(1.0f - texture(scene,TexCoords).rgb,1.0f);
	}
	else if (shake)
	{
		for (int i = 0; i < 9; i++)
			colour += vec4(samplers[i] * blur_kernel[i], 0.0f);
		colour.a = 1.0f;
	}
	else
	{
		colour = texture(scene,TexCoords);
	}

}