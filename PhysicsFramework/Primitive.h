#pragma once
#include <GL/glew.h>
// Base class for all components that can implement scene geometry
// A primitive is also used for geometry defined within the engine
// Use/Derive a Primitive component when defining new custom types of geometry
// TODO : [@Sai] - Make a PrimitiveFactory to create and supply basic shapes (cubes, capsules, spheres, etc.)
#include "Component.h"
#include "Subject.h"
#include "Vertex.h"

#include "Renderer.h"

class Primitive;

class PrimitiveEvent : public Event
{
public:
	enum EventList
	{
		TEXTURE_REQUEST,
		PrimitiveEventCount
	};
	EventList EventID;
	Primitive * EventOrigin;
	unsigned int TextureID;

	PrimitiveEvent() {};
	~PrimitiveEvent() {};
};

class Primitive : public Component
{
	/*----------ENUMS----------*/
public:
	enum PrimitiveType
	{
		SPRITE,
		MESH,
		PrimitiveCount
	};

	/*----------MEMBER VARIABLES----------*/
public:
	int PrimitiveSize;
	int SlotID;
	GLuint TBO;
	GLuint VAO;
	GLuint VBO;
	bool RenderDebug = true;
	std::vector<Vertex> Vertices;

	bool bIsBound;
	bool bIsDebug;
	PrimitiveType ePrimitiveType;
	Renderer::PrimitiveDataType ePrimitiveDataType;

	/*----------MEMBER FUNCTIONS----------*/
	// Called by child classes
	Primitive(PrimitiveType aType) : 
		Component(ComponentType::PRIMITIVE), 
		ePrimitiveType(aType) 
	{}

	// Called by component factory
	Primitive():
		Component(Component::PRIMITIVE)
	{}

	Primitive(Primitive const & CopyPrimitive) : 
		Component(ComponentType::PRIMITIVE) {}

	virtual ~Primitive() {};
	
	static inline ComponentType GetComponentID() { return (ComponentType::PRIMITIVE); }
	static inline const char * GetComponentName() { return ComponentTypeName[ComponentType::PRIMITIVE]; }

	inline PrimitiveType GetPrimitiveType() { return ePrimitiveType; }
	inline int GetPrimitiveSize() { return PrimitiveSize; }
	inline GLuint GetVAO() { return VAO; }
	inline GLuint GetVBO() { return VBO; }
	inline GLuint GetTBO() { return TBO; }

	inline void SetPrimitiveSize(int size) { PrimitiveSize = size; }
	inline void SetTBO(GLuint aTBO) { TBO = aTBO; }
	// Sets all vertex colors to a single color
	void SetVertexColorsUniform(glm::vec3 aNewColor);

	virtual void BindVertexData(std::vector<Vertex> & aVertexData);
	void ApplyTexture(unsigned int aTextureID);

	// Use move semantics to prevent a copy
	inline void SetVertices(std::vector<Vertex> & aVertexData) {
		Vertices = std::move(aVertexData);
		// Bind VAO with new vertices
		BindVertexData(Vertices);
	}

	glm::vec3 FindFarthestPointInDirection(glm::vec3 aDirection);

	// Used to send renderer requests for textures
	Subject TextureRequest;

	void Deserialize(TextFileData aTextData) {};
	void Update();
	// Unbuffers the vertex data
	void Debuffer();
};
