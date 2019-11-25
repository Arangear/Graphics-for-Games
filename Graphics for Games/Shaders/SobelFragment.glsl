#version 150 core
#define PI 3.1415926535897932384626433832795

uniform sampler2D diffuseTex;
uniform vec2 pixelSize;
uniform int stage;

in Vertex
{
	vec2 texCoord;
} IN;

out vec4 fragColour;

const float weights[5] = float[](0.12, 0.22, 0.32, 0.22, 0.12);

const float gradientX[9] = float[](	-1, 0, 1,
									-2, 0, 2,
									-1, 0, 1);

const float gradientY[9] = float[](	1, 2, 1,
									0, 0, 0,
									-1, -2, -1);

void main(void)
{
	if (stage != 2)
	{
		vec2 values[5];
		if (stage == 1)
		{
			values = vec2[](vec2(0.0, -pixelSize.y * 2), vec2(0.0, -pixelSize.y * 1), vec2(0.0, 0.0), vec2(0.0, pixelSize.y * 1), vec2(0.0, pixelSize.y * 2));
		}
		else
		{
			values = vec2[](vec2(-pixelSize.x * 2, 0.0), vec2(-pixelSize.x * 1, 0.0), vec2(0, 0.0), vec2(pixelSize.x * 1, 0.0), vec2(pixelSize.x * 2, 0.0));
		}

		for (int i = 0; i < 5; i++)
		{
			vec4 tmp = texture2D (diffuseTex, IN.texCoord.xy + values[i]);
			fragColour += tmp * weights[i];
		}
	}
	else
	{
		vec2 shift[9] = vec2[](	vec2(-pixelSize.x, pixelSize.y), vec2(0, pixelSize.y), vec2(pixelSize.x, pixelSize.y),
								vec2(-pixelSize.x, 0), vec2(0, 0), vec2(pixelSize.x, 0),
								vec2(-pixelSize.x, -pixelSize.y), vec2(0, -pixelSize.y), vec2(pixelSize.x, -pixelSize.y));

		vec3 gX = vec3(0.0f, 0.0f, 0.0f);
		vec3 gY = vec3(0.0f, 0.0f, 0.0f);

		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				int index = i * 3 + j;
				gX += vec3(	gradientX[index] * texture(diffuseTex, IN.texCoord + shift[index]).r,
							gradientX[index] * texture(diffuseTex, IN.texCoord + shift[index]).g,
							gradientX[index] * texture(diffuseTex, IN.texCoord + shift[index]).b);
				gY += vec3(	gradientY[index] * texture(diffuseTex, IN.texCoord + shift[index]).r,
							gradientY[index] * texture(diffuseTex, IN.texCoord + shift[index]).g,
							gradientY[index] * texture(diffuseTex, IN.texCoord + shift[index]).b);
			}
		}

		vec3 g = vec3(	sqrt(gX.r * gX.r + gY.r * gY.r),
						sqrt(gX.g * gX.g + gY.g * gY.g),
						sqrt(gX.b * gX.b + gY.b * gY.b));

		fragColour = vec4(g, 1);
	}
}
