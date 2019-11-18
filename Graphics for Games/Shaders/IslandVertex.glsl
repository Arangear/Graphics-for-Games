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
} OUT;

vec3 calculateNormal(mat3 normalMatrix);
vec3 calculateTangent(mat3 normalMatrix);

void main(void) 
{
	float h = texture(heightTex, heightCoord).r * heightMod;
	mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));

	mat4 mvp = projMatrix * viewMatrix * modelMatrix;
	OUT.texCoord = (textureMatrix * vec4(texCoord, 0.0, 1.0)).xy;
	OUT.height = h;
	OUT.normal = calculateNormal(normalMatrix);
	OUT.tangent = calculateTangent(normalMatrix);
	OUT.binormal = normalize(normalMatrix * normalize(cross(OUT.normal, OUT.tangent)));
	OUT.worldPos = (modelMatrix * vec4(position, 1)).xyz;

	gl_Position = mvp * vec4(position.x, h, position.z, 1.0);
}

vec3 calculateNormal(mat3 normalMatrix)
{
	float texMod = 1.0f / 1024.0f;
	float xSpacing = 16.0f;
	float zSpacing = 16.0f;
	float heightMultiplier = heightMod;

	vec3 result = vec3(0);
	//First triangle
	vec3 ba = vec3(	xSpacing,
					(texture(heightTex, vec2(heightCoord.x + texMod, heightCoord.y)).r - texture(heightTex, heightCoord).r) * heightMultiplier,
					0.0f);
	vec3 ca = vec3(	xSpacing,
					(texture(heightTex, vec2(heightCoord.x + texMod, heightCoord.y + texMod)).r - texture(heightTex, heightCoord).r) * heightMultiplier,
					zSpacing);
	result += cross(ca, ba);
	//Second triangle
	ba = vec3(	0.0f,
				(texture(heightTex, vec2(heightCoord.x, heightCoord.y + texMod)).r - texture(heightTex, heightCoord).r) * heightMultiplier,
				zSpacing);
	result += cross(ba, ca); 
	//Third triangle
	ba = vec3(	xSpacing,
				(texture(heightTex, heightCoord).r - texture(heightTex, vec2(heightCoord.x - texMod, heightCoord.y)).r) * heightMultiplier,
				0.0f);
	ca = vec3(	xSpacing,
				(texture(heightTex, vec2(heightCoord.x, heightCoord.y + texMod)).r - texture(heightTex, vec2(heightCoord.x - texMod, heightCoord.y)).r) * heightMultiplier,
				zSpacing);
	result += cross(ca, ba);
	//Fourth triangle
	ba = vec3(	0.0f,
				(texture(heightTex, vec2(heightCoord.x - texMod, heightCoord.y)).r - texture(heightTex, vec2(heightCoord.x - texMod, heightCoord.y - texMod)).r) * heightMultiplier,
				zSpacing);
	ca = vec3(	xSpacing,
				(texture(heightTex, heightCoord).r - texture(heightTex, vec2(heightCoord.x - texMod, heightCoord.y - texMod)).r) * heightMultiplier,
				zSpacing);
	result += cross(ba, ca);
	//Fifth triangle
	ba = vec3(	xSpacing,
				(texture(heightTex, vec2(heightCoord.x, heightCoord.y - texMod)).r - texture(heightTex, vec2(heightCoord.x - texMod, heightCoord.y - texMod)).r) * heightMultiplier,
				0.0f);
	result += cross(ca, ba);
	//Sixth triangle
	ba = vec3(	0.0f,
				(texture(heightTex, heightCoord).r - texture(heightTex, vec2(heightCoord.x, heightCoord.y - texMod)).r) * heightMultiplier,
				zSpacing);
	ca = vec3(	xSpacing,
				(texture(heightTex, vec2(heightCoord.x + texMod, heightCoord.y)).r - texture(heightTex, vec2(heightCoord.x, heightCoord.y - texMod)).r) * heightMultiplier,
				zSpacing);
	result += cross(ba, ca);

	return normalize(normalMatrix * result);
}

//TODO: Consult and improve
vec3 calculateTangent(mat3 normalMatrix)
{
	float texMod = 1.0f / 1024.0f;
	float xSpacing = 16.0f;
	float zSpacing = 16.0f;
	float heightMultiplier = heightMod;

	vec3 result = vec3(0);
	//First triangle
	vec2 tba = vec2(texMod, 0.0f);
	vec2 tca = vec2(texMod, texMod);
	vec3 ba = vec3(	xSpacing,
					(texture(heightTex, vec2(heightCoord.x + texMod, heightCoord.y)).r - texture(heightTex, heightCoord).r) * heightMultiplier,
					0.0f);
	vec3 ca = vec3(	xSpacing,
					(texture(heightTex, vec2(heightCoord.x + texMod, heightCoord.y + texMod)).r - texture(heightTex, heightCoord).r) * heightMultiplier,
					zSpacing);
	vec3 axis = vec3(ba * tca.y - ca * tba.y);
	float factor = 1.0f / (tba.x * tca.y - tca.x * tba.y);
	result += axis * factor;
	//Second triangle
	tba = vec2(0.0f, texMod);
	ba = vec3(0.0f,
		(texture(heightTex, vec2(heightCoord.x, heightCoord.y + texMod)).r - texture(heightTex, heightCoord).r) * heightMultiplier,
		zSpacing);
	axis = vec3(ba * tca.y - ca * tba.y);
	factor = 1.0f / (tba.x * tca.y - tca.x * tba.y);
	result += axis * factor;
	//Third triangle
	tba = vec2(texMod, 0.0f);
	ba = vec3(xSpacing,
		(texture(heightTex, heightCoord).r - texture(heightTex, vec2(heightCoord.x - texMod, heightCoord.y)).r) * heightMultiplier,
		0.0f);
	ca = vec3(xSpacing,
		(texture(heightTex, vec2(heightCoord.x, heightCoord.y + texMod)).r - texture(heightTex, vec2(heightCoord.x - texMod, heightCoord.y)).r) * heightMultiplier,
		zSpacing);
	axis = vec3(ba * tca.y - ca * tba.y);
	factor = 1.0f / (tba.x * tca.y - tca.x * tba.y);
	result += axis * factor;
	//Fourth triangle
	tba = vec2(0.0f, texMod);
	ba = vec3(0.0f,
		(texture(heightTex, vec2(heightCoord.x - texMod, heightCoord.y)).r - texture(heightTex, vec2(heightCoord.x - texMod, heightCoord.y - texMod)).r) * heightMultiplier,
		zSpacing);
	ca = vec3(xSpacing,
		(texture(heightTex, heightCoord).r - texture(heightTex, vec2(heightCoord.x - texMod, heightCoord.y - texMod)).r) * heightMultiplier,
		zSpacing);
	axis = vec3(ba * tca.y - ca * tba.y);
	factor = 1.0f / (tba.x * tca.y - tca.x * tba.y);
	result += axis * factor;
	//Fifth triangle
	tba = vec2(texMod, 0.0f);
	ba = vec3(xSpacing,
		(texture(heightTex, vec2(heightCoord.x, heightCoord.y - texMod)).r - texture(heightTex, vec2(heightCoord.x - texMod, heightCoord.y - texMod)).r) * heightMultiplier,
		0.0f);
	axis = vec3(ba * tca.y - ca * tba.y);
	factor = 1.0f / (tba.x * tca.y - tca.x * tba.y);
	result += axis * factor;
	//Sixth triangle
	tba = vec2(0.0f, texMod);
	ba = vec3(0.0f,
		(texture(heightTex, heightCoord).r - texture(heightTex, vec2(heightCoord.x, heightCoord.y - texMod)).r) * heightMultiplier,
		zSpacing);
	ca = vec3(xSpacing,
		(texture(heightTex, vec2(heightCoord.x + texMod, heightCoord.y)).r - texture(heightTex, vec2(heightCoord.x, heightCoord.y - texMod)).r) * heightMultiplier,
		zSpacing);
	axis = vec3(ba * tca.y - ca * tba.y);
	factor = 1.0f / (tba.x * tca.y - tca.x * tba.y);
	result += axis * factor;
	
	return normalize(normalMatrix * normalize(result));
}