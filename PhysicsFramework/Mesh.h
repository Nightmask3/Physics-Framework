#pragma once
#include "Primitive.h"
#include "Vertex.h"
class Mesh : public Primitive
{
	/*----------MEMBER VARIABLES----------*/
private:
	PrimitiveType ePrimitiveType;

	/*----------MEMBER VARIABLES----------*/
public:
	Mesh(GLuint aVAO, GLuint aVBO);
	virtual ~Mesh() {};

	PrimitiveType GetPrimitiveType() { return ePrimitiveType; }

	virtual void BindVertexData(std::vector<Vertex> & aVertexData);

	void Serialize(TextFileData aTextData);
	void Update() {};
};