#pragma once
#include "Primitive.h"
class Mesh : public Primitive
{
	/*----------MEMBER VARIABLES----------*/
private:
	PrimitiveType ePrimitiveType;
	/*----------MEMBER VARIABLES----------*/
public:
	Mesh(GLuint aVAO, GLuint aVBO);
	virtual ~Mesh() {};

	static ComponentType GetComponentID();

	PrimitiveType GetPrimitiveType() { return ePrimitiveType; }

	virtual void BindVertexData(std::vector<Vertex> & aVertexData);

	void Deserialize(TextFileData aTextData);
	void Update() {};
};