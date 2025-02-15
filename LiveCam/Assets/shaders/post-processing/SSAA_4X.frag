#version 450

uniform sampler2D Texture;
uniform vec2 samplesOffset[4];

in vec2 TexCoord;

out vec4 fragColor;

void main() 
{
	vec3 color = vec3(0, 0, 0);
	for (int i = 0; i < 4; ++i)
	{
		color += texture2D(Texture, TexCoord + samplesOffset[i]).rgb;
	}
	color /= 4;
	
	fragColor = vec4(color, 1.0);
}