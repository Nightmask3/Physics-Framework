#include <glm/glm.hpp>
#include "Box.h"
#include "ResourceManager.h"
#include "GameObject.h"
#include "Engine.h"
void Box::Initialize()
{
	// Loads the cube vertex positions/color data from file
	Vertices = std::move(pOwner->EngineHandle.GetResourceManager().ImportColliderData(std::string("Cube.fbx")));
}

glm::vec3 Box::FindFarthestPointInDirection(glm::vec3 aDirection)
{
	int index = 0, maxiumum = 0;
	maxiumum = glm::dot(aDirection, Vertices[0].Position);
	for (int i = 1; i < Vertices.size(); ++i)
	{
		float PositionProjectedAlongDirection = glm::dot(aDirection, Vertices[i].Position);
		if (PositionProjectedAlongDirection > maxiumum)
		{
			maxiumum = PositionProjectedAlongDirection;
			index = i;
		}
	}
	return Vertices[index].Position;
}
