#pragma once
#include "Primitive.h"

class Mesh : public Primitive
{
	/*----------MEMBER VARIABLES----------*/
private:
	PrimitiveType ePrimitiveType;

	/*----------MEMBER VARIABLES----------*/
public:
	Mesh(GLuint VAO, GLuint VBO);
	virtual ~Mesh() {};

	PrimitiveType GetPrimitiveType() { return ePrimitiveType; }

	virtual void BindVertexData(GLuint VAO, GLuint VBO);

	void Serialize(std::string &, unsigned int &) {};
	void Update() {};
};