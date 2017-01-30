#include "Primitive.h"

void Primitive::BindVertexData(std::vector<Vertex>& aVertexData)
{
	int primitiveSize = sizeof(Vertex) * aVertexData.size();
	SetPrimitiveSize(primitiveSize);
	/*--------------------------- VERTEX ARRAY OBJECT --------------------------------*/
	glBindVertexArray(VAO);
	/*--------------------------- VERTEX BUFFER OBJECT --------------------------------*/
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, primitiveSize, &aVertexData[0], GL_STATIC_DRAW);
	/*--------------------------- VERTEX ATTRIBUTE POINTERS --------------------------------*/
	// Position
	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(Vertex),
		(GLvoid*)0
	);
	glEnableVertexAttribArray(0);

	// Color
	glVertexAttribPointer(
		1,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(Vertex),
		(GLvoid*)(3 * sizeof(GLfloat))
	);
	glEnableVertexAttribArray(1);
	// Texture Coordinates
	glVertexAttribPointer(
		2,
		2,
		GL_FLOAT,
		GL_FALSE,
		sizeof(Vertex),
		(GLvoid*)(6 * sizeof(GLfloat))
	);
	glEnableVertexAttribArray(2);
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
