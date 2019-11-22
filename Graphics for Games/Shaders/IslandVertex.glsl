#version 150 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 textureMatrix;
uniform mat4 shadowMatrix;

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
	vec4 shadowProj[9];
	//vec4 shadowProj;
} OUT;

vec3 calculateTangent(mat3 normalMatrix, vec2 coord);
vec3 calculateBinormal(mat3 normalMatrix, vec2 coord);

void main(void) 
{
	float texMod = 1.0f / 512.0f;
	float xSpacing = 8.0f;
	float zSpacing = 8.0f;

	float h = texture(heightTex, heightCoord).r * heightMod;
	mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));

	mat4 mvp = projMatrix * viewMatrix * modelMatrix;
	OUT.texCoord = (textureMatrix * vec4(texCoord, 0.0, 1.0)).xy;
	OUT.height = h;
	
	
	for (int i = -1; i <= 1; i++)
	{
		for (int j = -1; j <= 1; j++)
		{
			vec2 location = vec2(heightCoord.x + i * texMod, heightCoord.y + j * texMod);

			vec3 tangent = calculateTangent(normalMatrix, location);
			vec3 binormal = calculateBinormal(normalMatrix, location);
			vec3 normal = normalize(normalMatrix * normalize(cross(tangent, binormal)));

			float height = texture(heightTex, location).r * heightMod;

			OUT.shadowProj[(i + 1) * 3 + j + 1] = shadowMatrix * modelMatrix * vec4(vec3(position.x + i * xSpacing, height, position.z + j * zSpacing) + (normal * 10), 1);
		}
	}
	

	OUT.tangent = calculateTangent(normalMatrix, heightCoord);
	OUT.binormal = calculateBinormal(normalMatrix, heightCoord);
	OUT.normal = normalize(normalMatrix * normalize(cross(OUT.tangent, OUT.binormal)));

	OUT.worldPos = (modelMatrix * vec4(position, 1)).xyz;

	//OUT.shadowProj = shadowMatrix * modelMatrix * vec4(vec3(position.x, h, position.z) + (OUT.normal * 10), 1);

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

vec3 calculateBinormal(mat3 normalMatrix, vec2 coord)
{
	float texMod = 1.0f / 512.0f;
	float xSpacing = 8.0f;
	float zSpacing = 8.0f;
	float heightMultiplier = heightMod;

	vec3 result = vec3(0);

	//CB
	result += vec3(	0,
					(texture(heightTex, vec2(coord.x + texMod, coord.y + texMod)).r - texture(heightTex, vec2(coord.x + texMod, coord.y)).r) * heightMultiplier,
					zSpacing);
	//DA
	result += vec3(	0,
					(texture(heightTex, vec2(coord.x, coord.y + texMod)).r - texture(heightTex, coord).r) * heightMultiplier,
					zSpacing);
	//DA
	result += vec3(	0,
					(texture(heightTex, vec2(coord.x, coord.y + texMod)).r - texture(heightTex, coord).r) * heightMultiplier,
					zSpacing);
	//EF
	result += vec3(	0,
					(texture(heightTex, vec2(coord.x - texMod, coord.y)).r - texture(heightTex, vec2(coord.x - texMod, coord.y - texMod)).r) * heightMultiplier,
					zSpacing);
	//AG
	result += vec3(	0,
					(texture(heightTex, vec2(coord.x, coord.y)).r - texture(heightTex, vec2(coord.x, coord.y - texMod)).r) * heightMultiplier,
					zSpacing);
	//AG
	result += vec3(	0,
					(texture(heightTex, vec2(coord.x, coord.y)).r - texture(heightTex, vec2(coord.x, coord.y - texMod)).r) * heightMultiplier,
					zSpacing);

	return normalize(normalMatrix * normalize(result));
}

vec3 calculateTangent(mat3 normalMatrix, vec2 coord)
{
	float texMod = 1.0f / 512.0f;
	float xSpacing = 8.0f;
	float zSpacing = 8.0f;
	float heightMultiplier = heightMod;

	vec3 result = vec3(0);

	//BA
	result += vec3(	xSpacing,
					(texture(heightTex, vec2(coord.x + texMod, coord.y)).r - texture(heightTex, coord).r) * heightMultiplier,
					0.0f);
	//CD
	result += vec3(	xSpacing,
					(texture(heightTex, vec2(coord.x + texMod, coord.y + texMod)).r - texture(heightTex, vec2(coord.x, coord.y + texMod)).r) * heightMultiplier,
					0.0f);
	//AE
	result += vec3(	xSpacing,
					(texture(heightTex, coord).r - texture(heightTex, vec2(coord.x - texMod, coord.y)).r) * heightMultiplier,
					0.0f);
	//AE
	result += vec3(	xSpacing,
					(texture(heightTex, coord).r - texture(heightTex, vec2(coord.x - texMod, coord.y)).r) * heightMultiplier,
					0.0f);
	//GF
	result += vec3(	xSpacing,
					(texture(heightTex, vec2(coord.x, coord.y - texMod)).r - texture(heightTex, vec2(coord.x - texMod, coord.y - texMod)).r) * heightMultiplier,
					0.0f);
	//BA
	result += vec3(	xSpacing,
					(texture(heightTex, vec2(coord.x + texMod, coord.y)).r - texture(heightTex, coord).r) * heightMultiplier,
					0.0f);

	return -normalize(normalMatrix * normalize(result));
}