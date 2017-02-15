#pragma once
#include <GL/glew.h>
// Base class for all components that can implement scene geometry
#include "Component.h"
#include "Subject.h"
#include "Vertex.h"
class PrimitiveEvent : public Event
{
public:
	enum EventList
	{
		TEXTURE_REQUEST,
		PrimitiveEventCount
	};
	EventList EventID;
	int TextureID;

	PrimitiveEvent() {};
	~PrimitiveEvent() {};
};

class Primitive : public Component
{
	/*----------MEMBER VARIABLES----------*/
public:
	enum PrimitiveType
	{
		SPRITE,
		MESH,
		MODEL,
		PrimitiveCount
	};

private:
	PrimitiveType ePrimitiveType;
	int PrimitiveSize;
	GLuint TBO;
	GLuint VAO;
	GLuint VBO;
	/*----------MEMBER VARIABLES----------*/
public:
	Primitive(PrimitiveType aType, GLuint aVAO, GLuint aVBO) : 
		Component(ComponentType::PRIMITIVE), 
		ePrimitiveType(aType), 
		VAO(aVAO), 
		VBO(aVBO) {}

	Primitive(Primitive const & CopyPrimitive) : 
		Component(ComponentType::PRIMITIVE) {}

	virtual ~Primitive() {};
	
	static ComponentType GetComponentID();

	inline PrimitiveType GetPrimitiveType() { return ePrimitiveType; }
	inline int GetPrimitiveSize() { return PrimitiveSize; }
	inline GLuint GetVAO() { return VAO; }
	inline GLuint GetVBO() { return VBO; }
	inline GLuint GetTBO() { return TBO; }
	inline void SetPrimitiveSize(int size) { PrimitiveSize = size; }
	inline void SetTBO(GLuint aTBO) { TBO = aTBO; }

	virtual void BindVertexData(std::vector<Vertex> & aVertexData);
	void ApplyTexture(unsigned int aTextureID);
	
	// Used to send renderer requests for textures
	Subject TextureRequest;

	void Deserialize(TextFileData aTextData) {};
	void Update();

};
