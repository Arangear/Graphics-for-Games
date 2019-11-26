//Author:			Daniel Cieslowski
//Last Modified:	26/11/2019
//Student No:		190562751
#version 150 core

uniform sampler2D sandTex;
uniform sampler2D rockTex;
uniform sampler2D dirtTex;
uniform sampler2DShadow shadowTex[2];
uniform float cascadeEnd[2];

uniform vec3 cameraPos;
uniform vec4 lightColour;
uniform vec3 lightPos;
uniform float lightRadius;

in Vertex 
{
	vec2 texCoord;
	float height;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos;
	vec4 shadowProj[18];
	float clipSpaceZ[9];
} IN;

out vec4 fragColour;

vec4 getDiffuse();

void main(void)
{
	vec4 diffuse = getDiffuse();

	mat3 TBN = mat3(IN.tangent, IN.binormal, IN.normal);

	vec3 incident = normalize(lightPos - IN.worldPos);
	float lambert = max(0.0, dot(incident, IN.normal));

	float dist = length(lightPos - IN.worldPos);
	float atten = 1.0 - clamp(dist / lightRadius, 0.0, 1.0);

	vec3 viewDir = normalize(cameraPos - IN.worldPos);
	vec3 halfDir = normalize(incident + viewDir);

	float rFactor = max(0.0, dot(halfDir, IN.normal));
	float sFactor = pow(rFactor, 10.0);

	float shadow = 1.0;
	float count = 0.0;
	float tmp = 0.0;
	for (int i = 0; i < 9; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			if (IN.clipSpaceZ[i] <= cascadeEnd[j])
			{
				if (IN.shadowProj[2 * i + j].w > 0.0)
				{
					tmp += textureProj(shadowTex[j], IN.shadowProj[2 * i + j]);
					count += 1.0;
				}
				break;
			}
		}
	}
	if (count != 0.0)
	{
		shadow = tmp / count;
	}
	lambert *= shadow;

	vec3 colour = diffuse.rgb * lightColour.rgb;
	colour += lightColour.rgb * sFactor * 0.1;
	fragColour = vec4(colour * atten * lambert, diffuse.a);
	fragColour.rgb += diffuse.rgb * lightColour.rgb * 0.1;
}

vec4 getDiffuse()
{
	vec4 result;
	if (IN.height < 500)
	{
		result = texture(sandTex, IN.texCoord);
	}
	else if (IN.height < 800)
	{
		result = mix(texture(dirtTex, IN.texCoord), texture(sandTex, IN.texCoord), (800.0f - IN.height) / 300.0f);
	}
	else if (IN.height < 900)
	{
		result = texture(dirtTex, IN.texCoord);
	}
	else if (IN.height < 1100)
	{
		result = mix(texture(rockTex, IN.texCoord), texture(dirtTex, IN.texCoord), (1100.0f - IN.height) / 200.0f);
	}
	else
	{
		result = texture(rockTex, IN.texCoord);
	}
	return result;
}