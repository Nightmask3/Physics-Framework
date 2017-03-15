#include "Primitive.h"
#include <glm/glm.hpp>

// Uses the simple method of looping through all vertices and projecting them along direction
// Return the vertex with the largest projection
glm::vec3 Primitive::FindFarthestPointInDirection(glm::vec3 aDirection)
{
	int index = 0, maxiumum = 0;
	maxiumum = glm::dot(aDirection, Vertices[0].Position);
	for (int i = 1; i < Vertices.size(); ++i)
	{
		float PositionProjectedAlongDirection = glm::dot(aDirection, Vertices[i].Position);
		if (PositionProjectedAlongDirection > maxiumum)
		{
			maxiumum = PositionProjectedAlongDirection;
			index = i;
		}
	}
	return Vertices[index].Position;
}


void Primitive::SetVertexColorsUniform(glm::vec3 aNewColor)
{
	for (int i = 0; i < Vertices.size(); ++i)
	{
		Vertices[i].Color = glm::vec4(aNewColor, 1);
	}
	// Rebuffer the VBO once colors have been changed
	BindVertexData(Vertices);
}

void Primitive::BindVertexData(std::vector<Vertex>& aVertexData)
{
	int primitiveSize = sizeof(Vertex) * (int)aVertexData.size();
	SetPrimitiveSize(primitiveSize);
	/*--------------------------- VERTEX ARRAY OBJECT --------------------------------*/
	glBindVertexArray(VAO);
	/*--------------------------- VERTEX BUFFER OBJECT --------------------------------*/
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, primitiveSize, &aVertexData[0], GL_STATIC_DRAW);
	/*--------------------------- VERTEX ATTRIBUTE POINTERS --------------------------------*/
	// Position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(Vertex),
		(GLvoid*)0
	);

	// Normal
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(Vertex),
		(GLvoid*)(3 * sizeof(GLfloat))
	);
	
	// Color
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(
		2,
		4,
		GL_FLOAT,
		GL_FALSE,
		sizeof(Vertex),
		(GLvoid*)(6 * sizeof(GLfloat))
	);
	
	// Texture Coordinates
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(
		3,
		2,
		GL_FLOAT,
		GL_FALSE,
		sizeof(Vertex),
		(GLvoid*)(10 * sizeof(GLfloat))
	);

	// Unbind VAO
	glBindVertexArray(0);
}

void Primitive::ApplyTexture(unsigned int aTextureID)
{
	PrimitiveEvent TextureEvent;
	TextureEvent.EventID = PrimitiveEvent::TEXTURE_REQUEST;
	TextureEvent.TextureID = aTextureID;
	TextureRequest.Notify(this, &TextureEvent);
}

void Primitive::Update()
{
}
