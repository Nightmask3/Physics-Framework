#pragma once
#include "Primitive.h"
class Mesh : public Primitive
{
	/*----------MEMBER VARIABLES----------*/
public:
	Mesh();
	virtual ~Mesh() {};

	static ComponentType GetComponentID();

	PrimitiveType GetPrimitiveType() { return ePrimitiveType; }

	void Deserialize(TextFileData aTextData);
	void Update() {};
};