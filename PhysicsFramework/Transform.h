#pragma once
#include "Component.h"
#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <algorithm>
#include <iterator>
#include <cassert>
#include <sstream>
// Math header files
#include "glm/mat3x3.hpp"
#include "glm/vec3.hpp"
// Component header files
#include "Component.h"

class Transform : public Component
{
	/*----------MEMBER VARIABLES----------*/
private:
	glm::vec3 Position;			
	glm::vec3 Rotation;			
	glm::vec3 Scale;			
	glm::vec3 OriginalScale;

	/*----------MEMBER FUNCTIONS----------*/
public:
	Transform() : Component(ComponentType::TRANSFORM), Position(glm::vec3(0)), Rotation(glm::vec3(0)), Scale(glm::vec3(1)) {}
	Transform(Transform const & CopyTransform) : Component(ComponentType::TRANSFORM) 
	{ 
		Position = CopyTransform.Position;
		Rotation = CopyTransform.Rotation;
		Scale = CopyTransform.Scale;
	}
	virtual ~Transform() {};

	static Component::ComponentType GetComponentID();

	inline glm::vec3 GetPosition() { return Position; }
	inline glm::vec3 GetRotation() { return Rotation; }
	inline glm::vec3 GetScale() { return Scale; }

	inline void SetPosition(glm::vec3 newPosition) { Position = newPosition; }
	inline void SetRotation(glm::vec3 newRotation) { Rotation = newRotation; }
	inline void SetScale(glm::vec3 newScale) { Scale = newScale; }
	
	virtual void Deserialize(TextFileData aTextFileData) override;
	void Update();
};

