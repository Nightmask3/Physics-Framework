#pragma once
#include <GL/glew.h>
// Base class for all components that can implement scene geometry
#include "Component.h"
#include "Subject.h"

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
	/*----------MEMBER VARIABLES----------*/
public:
	Primitive(PrimitiveType type, GLuint VAO, GLuint VBO) : Component(ComponentType::PRIMITIVE), ePrimitiveType(type) {}
	Primitive(Primitive const & CopyPrimitive) : Component(ComponentType::PRIMITIVE) {}
	virtual ~Primitive() {};
	
	inline PrimitiveType GetPrimitiveType() { return ePrimitiveType; }
	inline int GetPrimitiveSize() { return PrimitiveSize; }
	inline GLuint GetTBO() { return TBO; }
	inline void SetPrimitiveSize(int size) { PrimitiveSize = size; }
	inline void SetTBO(GLuint aTBO) { TBO = aTBO; }

	virtual void BindVertexData(GLuint VAO, GLuint VBO) = 0;
	void ApplyTexture(unsigned int aTextureID);
	
	Subject TextureRequest;

	void Serialize(std::string &, unsigned int &) {};
	void Update();

};
