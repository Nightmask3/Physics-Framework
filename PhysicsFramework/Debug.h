#pragma once
#include "Primitive.h"

class Debug : public Primitive
{
	/*----------MEMBER VARIABLES----------*/
private:
	PrimitiveType ePrimitiveType;

	/*----------MEMBER VARIABLES----------*/
public:
	Debug(GLuint VAO, GLuint VBO);
	virtual ~Debug() {};

	static ComponentType GetComponentID();

	PrimitiveType GetPrimitiveType() { return ePrimitiveType; }

	void Deserialize(char * aTextData) {};
	void Update() {};
};