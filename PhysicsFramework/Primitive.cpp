#include "Primitive.h"

inline Component::ComponentType Primitive::GetComponentID()
{
	return (ComponentType::PRIMITIVE);
}

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
