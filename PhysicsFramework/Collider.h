#pragma once
#include "Component.h"
#include "DebugVertex.h"

// Abstract base class for any component that implements a collision shape (box, capsule, sphere, etc) 
class Collider : public Component
{
	/*-----------MEMBER VARIABLES-----------*/
public:
	bool bIsCollisionEnabled;
	bool bShouldRenderDebug;
	std::vector<DebugVertex> Vertices;
	/*-----------MEMBER FUNCTIONS-----------*/
public:
	Collider() : 
		Component(Component::COLLIDER) {}
	~Collider() {}

	virtual glm::vec3 FindFarthestPointInDirection(glm::vec3 aDirection) = 0;
	virtual void Update() {};
	virtual void Deserialize(TextFileData aTextData) {};

};