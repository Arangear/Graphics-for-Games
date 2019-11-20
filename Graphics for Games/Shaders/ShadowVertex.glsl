#version 150 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

in vec3 position;
in vec2 heightCoord;
uniform float heightMod;
uniform sampler2D heightTex;

void main(void)
{
	float height = texture(heightTex, heightCoord) * heightMod;
	gl_Position = (projMatrix * viewMatrix * modelMatrix) * vec4(position.x, height, position.z, 1.0);
}