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

	static ComponentType GetComponentID();

	std::vector<Vertex> Vertices;

	// Use move semantics to prevent a copy
	inline void SetVertices(std::vector<Vertex> & aVertexData) {
		Vertices = std::move(aVertexData);
		// Bind VAO with new vertices
		BindVertexData(Vertices);
	}

	PrimitiveType GetPrimitiveType() { return ePrimitiveType; }

	virtual void BindVertexData(std::vector<Vertex> & aVertexData);

	void Deserialize(TextFileData aTextData);
	void Update() {};
};