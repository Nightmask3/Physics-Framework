#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include "UtilityFunctions.h"
#include "Transform.h"
#include "GameObject.h"

// aDirection doesn't need to be normalized
SupportPoint Utility::Support(Primitive * aShape1, Primitive * aShape2, glm::vec3 aDirection)
{
	SupportPoint newSupportPoint;

	// Get points on edge of the shapes in opposite directions, in object space
	newSupportPoint.OS_SupportPointA = aShape1->FindFarthestPointInDirection(aDirection);
	newSupportPoint.OS_SupportPointB = aShape2->FindFarthestPointInDirection(-aDirection);

	// Calculate the model matrices and convert the farthest points to world space
	glm::mat4 model1, model2;
	glm::mat4 translate, scale;
	translate = glm::translate(aShape1->GetOwner()->GetComponent<Transform>()->GetPosition());
	scale = glm::scale(aShape1->GetOwner()->GetComponent<Transform>()->GetScale());
	model1 = translate * scale;

	translate = glm::translate(aShape2->GetOwner()->GetComponent<Transform>()->GetPosition());
	scale = glm::scale(aShape2->GetOwner()->GetComponent<Transform>()->GetScale());
	model2 = translate * scale;

	newSupportPoint.WS_SupportPointA = glm::vec3(model1 * glm::vec4(newSupportPoint.OS_SupportPointA, 1));
	newSupportPoint.WS_SupportPointB = glm::vec3(model2 * glm::vec4(newSupportPoint.OS_SupportPointB, 1));

	// Perform the Minkowski Difference
	newSupportPoint.MinkowskiHullVertex = newSupportPoint.WS_SupportPointA - newSupportPoint.WS_SupportPointB;
	return newSupportPoint;
}
