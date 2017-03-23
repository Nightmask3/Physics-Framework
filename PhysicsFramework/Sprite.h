#pragma once
#include "Primitive.h"

class Sprite : public Primitive
{
	/*----------MEMBER VARIABLES----------*/
public:
	Sprite();
	virtual ~Sprite() {};

	static ComponentType GetComponentID();

	PrimitiveType GetPrimitiveType() { return ePrimitiveType; }

	virtual void BindVertexData(std::vector<Vertex> & aVertexData);

	void Deserialize(char * aTextData) {};
	void Update() {};
};