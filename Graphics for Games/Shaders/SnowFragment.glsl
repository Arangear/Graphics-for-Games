#version 150 core

uniform sampler2D diffuseTex;
uniform float maxHeight;
uniform float minHeight;

in Vertex 
{
	vec2 texCoord;
	float height;
} IN;

out vec4 gl_FragColor;

void main(void)
{
	gl_FragColor = mix(texture(diffuseTex, IN.texCoord), vec4(1,1,1,1), (IN.height - minHeight) / (maxHeight - minHeight));
}