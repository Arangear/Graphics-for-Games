#version 150 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 textureMatrix;

uniform float heightMod;
uniform sampler2D heightTex;

in vec3 position;
in vec2 texCoord;
in vec2 heightCoord;

out Vertex
{
	vec2 texCoord;
	float height;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos;
	vec4 shadowProj;
} OUT;

vec3 calculateTangent(mat3 normalMatrix);
vec3 calculateBinormal(mat3 normalMatrix);

void main(void) 
{
	float h = texture(heightTex, heightCoord).r * heightMod;
	mat3 normalMatrix = transpose(inverse(mat3(modelMatrix))); // modelMatrix for Island should be Identity => normalMatrix should be Identity (verified)

	mat4 mvp = projMatrix * viewMatrix * modelMatrix;
	OUT.texCoord = (textureMatrix * vec4(texCoord, 0.0, 1.0)).xy; // textureMatrix for Island should be Identity (verified)
	OUT.height = h;
	
	OUT.tangent = calculateTangent(normalMatrix);
	OUT.binormal = calculateBinormal(normalMatrix);
	OUT.normal = normalize(normalMatrix * normalize(cross(OUT.tangent, OUT.binormal)));

	/*
	OUT.tangent = normalize(normalMatrix * vec3(1, 0, 0));
	OUT.binormal = normalize(normalMatrix * vec3(0, 0, -1));
	OUT.normal = normalize(normalMatrix * vec3(0, 1, 0));
	*/
	OUT.worldPos = (modelMatrix * vec4(position, 1)).xyz;
	//OUT.shadowProj = (textureMatrix * vec4(position + (vec3(0, 1, 0) * 1.5), 1)); // w component should always be 1 (true since textureMatrix is Identity)
	OUT.shadowProj = (textureMatrix * vec4(vec3(position.x, h, position.z) + (cross(OUT.tangent, OUT.binormal) * 1.5), 1));

	//gl_Position = mvp * vec4(position, 1.0);
	gl_Position = mvp * vec4(position.x, h, position.z, 1.0);
}

//             D----C
//            /|   /|
//           / |  / |
//          /  | /  |
//         /   |/   |
//        E----A----B
//        |   /|   /
//        |  / |  /
//        | /  | /
//        |/   |/
//        F----G

vec3 calculateBinormal(mat3 normalMatrix)
{
	float texMod = 1.0f / 512.0f;
	float xSpacing = 8.0f;
	float zSpacing = 8.0f;
	float heightMultiplier = heightMod;

	vec3 result = vec3(0);

	//CB
	result += vec3(	0,
					(texture(heightTex, vec2(heightCoord.x + texMod, heightCoord.y + texMod)).r - texture(heightTex, vec2(heightCoord.x + texMod, heightCoord.y)).r) * heightMultiplier,
					zSpacing);
	//DA
	result += vec3(	0,
					(texture(heightTex, vec2(heightCoord.x, heightCoord.y + texMod)).r - texture(heightTex, heightCoord).r) * heightMultiplier,
					zSpacing);
	//DA
	result += vec3(	0,
					(texture(heightTex, vec2(heightCoord.x, heightCoord.y + texMod)).r - texture(heightTex, heightCoord).r) * heightMultiplier,
					zSpacing);
	//EF
	result += vec3(	0,
					(texture(heightTex, vec2(heightCoord.x - texMod, heightCoord.y)).r - texture(heightTex, vec2(heightCoord.x - texMod, heightCoord.y - texMod)).r) * heightMultiplier,
					zSpacing);
	//AG
	result += vec3(	0,
					(texture(heightTex, vec2(heightCoord.x, heightCoord.y)).r - texture(heightTex, vec2(heightCoord.x, heightCoord.y - texMod)).r) * heightMultiplier,
					zSpacing);
	//AG
	result += vec3(	0,
					(texture(heightTex, vec2(heightCoord.x, heightCoord.y)).r - texture(heightTex, vec2(heightCoord.x, heightCoord.y - texMod)).r) * heightMultiplier,
					zSpacing);

	return normalize(normalMatrix * normalize(result));
}

vec3 calculateTangent(mat3 normalMatrix)
{
	float texMod = 1.0f / 512.0f;
	float xSpacing = 8.0f;
	float zSpacing = 8.0f;
	float heightMultiplier = heightMod;

	vec3 result = vec3(0);

	//BA
	result += vec3(	xSpacing,
					(texture(heightTex, vec2(heightCoord.x + texMod, heightCoord.y)).r - texture(heightTex, heightCoord).r) * heightMultiplier,
					0.0f);
	//CD
	result += vec3(	xSpacing,
					(texture(heightTex, vec2(heightCoord.x + texMod, heightCoord.y + texMod)).r - texture(heightTex, vec2(heightCoord.x, heightCoord.y + texMod)).r) * heightMultiplier,
					0.0f);
	//AE
	result += vec3(	xSpacing,
					(texture(heightTex, heightCoord).r - texture(heightTex, vec2(heightCoord.x - texMod, heightCoord.y)).r) * heightMultiplier,
					0.0f);
	//AE
	result += vec3(	xSpacing,
					(texture(heightTex, heightCoord).r - texture(heightTex, vec2(heightCoord.x - texMod, heightCoord.y)).r) * heightMultiplier,
					0.0f);
	//GF
	result += vec3(	xSpacing,
					(texture(heightTex, vec2(heightCoord.x, heightCoord.y - texMod)).r - texture(heightTex, vec2(heightCoord.x - texMod, heightCoord.y - texMod)).r) * heightMultiplier,
					0.0f);
	//BA
	result += vec3(	xSpacing,
					(texture(heightTex, vec2(heightCoord.x + texMod, heightCoord.y)).r - texture(heightTex, heightCoord).r) * heightMultiplier,
					0.0f);

	return -normalize(normalMatrix * normalize(result));
}