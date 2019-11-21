#include "Island.h"

Island::Island()
{
	numVertices = WIDTH * HEIGHT;
	numIndices = (WIDTH - 1) * (HEIGHT - 1) * 6;
	vertices = new Vector3[numVertices];
	textureCoords = new Vector2[numVertices];
	heightCoords = new Vector2[numVertices];
	indices = new GLuint[numIndices];

	for (int x = 0; x < WIDTH; ++x)
	{
		for (int z = 0; z < HEIGHT; ++z)
		{
			int offset = (x * WIDTH) + z;
			vertices[offset] = Vector3((x - WIDTH / 2) * HEIGHTMAP_X, 0, (z - HEIGHT / 2) * HEIGHTMAP_Z);
			textureCoords[offset] = Vector2(x * HEIGHTMAP_TEX_X, z * HEIGHTMAP_TEX_Z);
			heightCoords[offset] = Vector2(x * HEIGHT_TEX_X, z * HEIGHT_TEX_Z);
		}
	}
	numIndices = 0;

	for (int x = 0; x < WIDTH - 1; ++x)
	{
		for (int z = 0; z < HEIGHT - 1; ++z)
		{
			int a = (x * (WIDTH)) + z;
			int b = ((x + 1) * (WIDTH)) + z;
			int c = ((x + 1) * (WIDTH)) + (z + 1);
			int d = (x * (WIDTH)) + (z + 1);

			indices[numIndices++] = c;
			indices[numIndices++] = b;
			indices[numIndices++] = a;

			indices[numIndices++] = a;
			indices[numIndices++] = d;
			indices[numIndices++] = c;
		}
	}

	GenerateNormals();
	GenerateTangents();

	BufferAllData();
}

Island::~Island()
{
	glDeleteTextures(1, &heightTexture);
	glDeleteTextures(1, &sandTexture);
	glDeleteTextures(1, &dirtTexture);
	glDeleteTextures(1, &rockTexture);
	delete[] heightCoords;
}

void Island::Draw()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, heightTexture);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, sandTexture);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, rockTexture);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, dirtTexture);

	glBindVertexArray(arrayObject);
	if (bufferObject[INDEX_BUFFER])
	{
		glDrawElements(type, numIndices, GL_UNSIGNED_INT, 0);
	}
	else
	{
		glDrawArrays(type, 0, numVertices);
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Island::BufferAllData()
{
	glBindVertexArray(arrayObject);
	glGenBuffers(1, &bufferObject[VERTEX_BUFFER]);
	glBindBuffer(GL_ARRAY_BUFFER, bufferObject[VERTEX_BUFFER]);
	glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vector3), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(VERTEX_BUFFER, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(VERTEX_BUFFER);
	if (textureCoords)
	{
		glGenBuffers(1, &bufferObject[TEXTURE_BUFFER]);
		glBindBuffer(GL_ARRAY_BUFFER, bufferObject[TEXTURE_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vector2), textureCoords, GL_STATIC_DRAW);
		glVertexAttribPointer(TEXTURE_BUFFER, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(TEXTURE_BUFFER);
	}
	if (colours)
	{
		glGenBuffers(1, &bufferObject[COLOUR_BUFFER]);
		glBindBuffer(GL_ARRAY_BUFFER, bufferObject[COLOUR_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vector4), colours, GL_STATIC_DRAW);
		glVertexAttribPointer(COLOUR_BUFFER, 4, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(COLOUR_BUFFER);
	}
	if (indices)
	{
		glGenBuffers(1, &bufferObject[INDEX_BUFFER]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferObject[INDEX_BUFFER]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(GLuint), indices, GL_STATIC_DRAW);
	}
	if (heightCoords)
	{
		glGenBuffers(1, &bufferObject[HEIGHT_BUFFER]);
		glBindBuffer(GL_ARRAY_BUFFER, bufferObject[HEIGHT_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vector2), heightCoords, GL_STATIC_DRAW);
		glVertexAttribPointer(HEIGHT_BUFFER, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(HEIGHT_BUFFER);
	}

	glBindVertexArray(0);
}
