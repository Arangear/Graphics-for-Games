//Author:			Daniel Cieslowski
//Last Modified:	25/11/2019
//Student No:		190562751
#version 150 core

uniform sampler2D diffuseTex;

in Vertex 
{
	vec2 texCoord;
} IN;

out vec4 fragColor;

void main(void)
{
	fragColor = texture(diffuseTex, IN.texCoord);
}