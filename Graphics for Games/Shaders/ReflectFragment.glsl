//Author:			Daniel Cieslowski
//Last Modified:	25/11/2019
//Student No:		190562751
#version 150 core

uniform sampler2D diffuseTex;
uniform samplerCube cubeTex;

uniform vec3 cameraPos;
uniform vec4 lightColour;
uniform vec3 lightPos;
uniform float lightRadius;

in Vertex
{
	vec4 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 worldPos;
} IN;

out vec4 fragColour;

void main(void)
{
	vec4 diffuse = texture(diffuseTex, IN.texCoord) * vec4(IN.colour.rgb, 0.6);
	vec3 incident = normalize(IN.worldPos - cameraPos);

	float dist = length(lightPos - IN.worldPos);
	float atten = 1.0 - clamp(dist / lightRadius, 0.2, 1.0);

	vec4 reflection = texture(cubeTex, reflect(incident, normalize(IN.normal)));

	fragColour = (lightColour * diffuse * atten)* (diffuse + reflection);
}