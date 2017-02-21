#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include "UtilityFunctions.h"
#include "Transform.h"
#include "GameObject.h"

// aDirection doesn't need to be normalized
SupportPoint Utility::Support(Primitive * aaShape1, Primitive * aaShape2, glm::vec3 aDirection)
{
	SupportPoint newSupportPoint;

	// Get points on edge of the shapes in opposite directions, in object space
	newSupportPoint.OS_SupportPointA = aaShape1->FindFarthestPointInDirection(aDirection);
	newSupportPoint.OS_SupportPointB = aaShape2->FindFarthestPointInDirection(-aDirection);

	// Calculate the model matrices and convert the farthest points to world space
	glm::mat4 model1, model2;
	glm::mat4 translate, scale;
	translate = glm::translate(aaShape1->GetOwner()->GetComponent<Transform>()->GetPosition());
	scale = glm::scale(aaShape1->GetOwner()->GetComponent<Transform>()->GetScale());
	model1 = translate * scale;

	translate = glm::translate(aaShape2->GetOwner()->GetComponent<Transform>()->GetPosition());
	scale = glm::scale(aaShape2->GetOwner()->GetComponent<Transform>()->GetScale());
	model2 = translate * scale;

	newSupportPoint.WS_SupportPointA = glm::vec3(model1 * glm::vec4(newSupportPoint.OS_SupportPointA, 1));
	newSupportPoint.WS_SupportPointB = glm::vec3(model2 * glm::vec4(newSupportPoint.OS_SupportPointB, 1));

	// Perform the Minkowski Difference
	newSupportPoint.MinkowskiHullVertex = newSupportPoint.WS_SupportPointA - newSupportPoint.WS_SupportPointB;
	return newSupportPoint;
}

void Utility::CalculateMinkowskiDifference(std::vector<Vertex>& aMinkowskiDifference, Mesh * aShape1, Mesh * aShape2)
{
	int size1 = aShape1->Vertices.size();
	int size2 = aShape2->Vertices.size();
	std::vector<Vertex> MinkowskiDifferenceVertices;

	for (int i = 0; i < size1; ++i)
	{
		// Calculate the model matrices and set the matrix uniform
		glm::mat4 model1, model2;
		glm::mat4 translate, scale;
		translate = glm::translate(aShape1->GetOwner()->GetComponent<Transform>()->GetPosition());
		scale = glm::scale(aShape1->GetOwner()->GetComponent<Transform>()->GetScale());
		model1 = translate * scale;

		translate = glm::translate(aShape2->GetOwner()->GetComponent<Transform>()->GetPosition());
		scale = glm::scale(aShape2->GetOwner()->GetComponent<Transform>()->GetScale());
		model2 = translate * scale;

		glm::vec4 position1 = model1 * glm::vec4(aShape1->Vertices[i].Position, 1);
		glm::vec4 position2 = model2 * glm::vec4(aShape2->Vertices[i].Position, 1);
		Vertex newVertex;
		newVertex.Position = glm::vec3(position2 - position1);
		MinkowskiDifferenceVertices.push_back(newVertex);
	}

	// Sort Minkowski Difference vertices
	// https://stackoverflow.com/questions/6880899/sort-a-set-of-3-d-points-in-clockwise-counter-clockwise-order
	glm::vec3 zAxis = glm::vec3(0, 0, 1);
	glm::vec3 xAxis = glm::vec3(1, 0, 0);

	std::vector<std::pair<int, float>> order(MinkowskiDifferenceVertices.size());

	for (int i = 0; i < MinkowskiDifferenceVertices.size(); ++i)
	{
		float zAngle = glm::dot(MinkowskiDifferenceVertices[i].Position, zAxis);
		float xAngle = glm::dot(MinkowskiDifferenceVertices[i].Position, xAxis);
		float angle = atan2(zAngle, xAngle);
		order[i] = std::make_pair(i, angle);
	}

	std::sort(order.begin(), order.end(), Utility::FloatOrdering());
	Utility::SortUsingOrderingPairs(MinkowskiDifferenceVertices, order);
	
	aMinkowskiDifference = std::move(MinkowskiDifferenceVertices);
}
