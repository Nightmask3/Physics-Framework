#include <glm/vec3.hpp>
#include <glm/mat3x3.hpp>
#include "Box.h"
#include "ResourceManager.h"
#include "GameObject.h"
#include "Physics.h"
#include "Transform.h"
#include "Engine.h"

void Box::Initialize()
{
	// Loads the cube vertex positions/color data from file
	Vertices = std::move(pOwner->EngineHandle.GetResourceManager().ImportColliderData(std::string("Cube.fbx")));
	Physics & physics = *pOwner->GetComponent<Physics>();
	Transform & transform = *pOwner->GetComponent<Transform>();
	float mass = physics.Mass;
	glm::mat3 & intertiaTensor = physics.InertiaTensor;
	float width = Side * transform.Scale.x;
	float height = Side * transform.Scale.y;
	float length = Side * transform.Scale.z;

	// Initialize inertia tensor of a symmetric rectangular prism in body space:
	// http://www-robotics.cs.umass.edu/~grupen/603/slides/DynamicsI.pdf
	intertiaTensor[0][0] = (mass / 12) * (pow(length, 2) + pow(height, 2));
	intertiaTensor[0][1] = 0;
	intertiaTensor[0][2] = 0;

	intertiaTensor[1][0] = 0;
	intertiaTensor[1][1] = (mass / 12) * (pow(width, 2) + pow(height, 2));
	intertiaTensor[1][2] = 0;

	intertiaTensor[2][0] = 0;
	intertiaTensor[2][1] = 0;
	intertiaTensor[2][2] = (mass / 12) * (pow(length, 2) + pow(width, 2));

	// Calculate inverse inertia tensor and store it as well:
	physics.InverseInertiaTensor = glm::inverse(intertiaTensor);

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
