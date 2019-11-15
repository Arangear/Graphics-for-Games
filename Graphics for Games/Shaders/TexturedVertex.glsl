#version 150 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 textureMatrix;

uniform sampler2D heightTex;

in vec3 position;
in vec2 texCoord;
in vec2 heightCoord;

out Vertex
{
	vec2 texCoord;
	float height;
} OUT;

void main(void) 
{
	float h = texture(heightTex, heightCoord).r * 5000.f;

	mat4 mvp = projMatrix * viewMatrix * modelMatrix;
	gl_Position = mvp * vec4(position.x, h, position.z, 1.0);
	OUT.texCoord = (textureMatrix * vec4(texCoord, 0.0, 1.0)).xy;
	OUT.height = h;
}