#pragma once
#include "Primitive.h"

class Sprite : public Primitive
{
	/*----------MEMBER VARIABLES----------*/
private:
	PrimitiveType ePrimitiveType;

	/*----------MEMBER VARIABLES----------*/
public:
	Sprite(GLuint VAO, GLuint VBO);
	virtual ~Sprite() {};

	PrimitiveType GetPrimitiveType() { return ePrimitiveType; }

	virtual void BindVertexData(std::vector<Vertex> & aVertexData);

	void Serialize(char * aTextData) {};
	void Update() {};
};