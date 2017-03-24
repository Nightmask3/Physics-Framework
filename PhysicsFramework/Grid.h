#pragma once
#include <GL/glew.h>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include "DebugVertex.h"

struct Grid
{
	int SectionsX;
	int SectionsY;
	float SizeX;
	float SizeY;
	std::vector<float> HeightValues;
	std::vector<Vertex> GridVertices;
	glm::vec3 Color = glm::vec3(1, 0, 0);
	GLuint VAO;
	GLuint VBO;

	/* Argument 1 - Sections X
	 Argument 2 - Sections Y
	 Argument 3 - Size X
	 Argument 4 - Size Y */
	Grid(int aSectionsX, int aSectionsY, int aSizeX, int aSizeY)
		:
		SectionsX(aSectionsX),
		SectionsY(aSectionsY),
		SizeX(aSizeX),
		SizeY(aSizeY)
	{
		HeightValues.reserve(SectionsX * SectionsY);
		for (int i = 0; i < SectionsX * SectionsY; ++i)
		{
			HeightValues.push_back(0.0f);
		}
		CalculateGrid();
	}
	~Grid() {}
	inline void CalculateGrid()
	{
		GridVertices.clear();

		for (int i = 0; i < SectionsX; ++i)
		{
			for (int j = 0; j < SectionsY; ++j)
			{
				float intervalX = SizeX / SectionsX;
				float intervalY = SizeY / SectionsY;
				int index = i * SectionsY + j;
				Vertex newGridVertex;
				newGridVertex.Color = glm::vec4(Color, 1);
				// Triangle 1
				// Add grid vertex to vertex array
				newGridVertex.Position = glm::vec3(i * intervalX, HeightValues[index], j * intervalY);
				GridVertices.push_back(newGridVertex);
				// Add grid vertex to vertex array
				newGridVertex.Position = glm::vec3((i + 1) * intervalX, HeightValues[index], j * intervalY);
				GridVertices.push_back(newGridVertex);
				// Add grid vertex to vertex array
				newGridVertex.Position = glm::vec3(i * intervalX, HeightValues[index], (j + 1) * intervalY);
				GridVertices.push_back(newGridVertex);

				// Triangle 2
				// Add grid vertex to vertex array
				newGridVertex.Position = glm::vec3((i + 1) * intervalX, HeightValues[index], (j + 1) * intervalY);
				GridVertices.push_back(newGridVertex);
				// Add grid vertex to vertex array
				newGridVertex.Position = glm::vec3((i + 1) * intervalX, HeightValues[index], j * intervalY);
				GridVertices.push_back(newGridVertex);
				// Add grid vertex to vertex array
				newGridVertex.Position = glm::vec3(i * intervalX, HeightValues[index], (j + 1) * intervalY);
				GridVertices.push_back(newGridVertex);
			}
		}
	}
};