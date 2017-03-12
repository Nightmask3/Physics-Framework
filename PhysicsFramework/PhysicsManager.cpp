#include <list>

#include "PhysicsManager.h"
#include "Renderer.h"
#include "InputManager.h"
#include "FrameRateController.h"

#include "GameObject.h"
#include "Transform.h"

#include "UtilityFunctions.h"

int PhysicsManager::Iterations = 10;

void PhysicsManager::Update()
{
	if (EngineHandle.GetInputManager().isKeyPressed(GLFW_KEY_KP_ADD) && Iterations <= 200)
		std::cout<<"New Iteration Count:" << ++Iterations << std::endl;
	else if(EngineHandle.GetInputManager().isKeyPressed(GLFW_KEY_KP_SUBTRACT) && Iterations >= 1)
		std::cout << "New Iteration Count:" << --Iterations << std::endl;

	// Three Stages
	// Simulation : Update the physics variables of all objects that have physics components (Apply gravity, velocity, forces etc.)
	Simulation();

	// Collision Detection : Checking every Physics Component that has a Rigid Body for collision against every other rigid body
	DetectCollision();

	// Resolution: Apply the physics values to the game object transforms
	//Resolution();
}

void PhysicsManager::DetectCollision()
{
	// Do collision detection for each pair of objects
	for (int i = 0; i < PhysicsObjectsList.size(); ++i)
	{
		for (int j = 0; j < PhysicsObjectsList.size(); ++j)
		{
			if( i == j )
				break;
			Physics * physicsObject1 = PhysicsObjectsList[i];
			Physics * physicsObject2 = PhysicsObjectsList[j];
			ContactData newContactData;
			// Calculate the model matrices and store in contact data for future use
			glm::mat4 model1, model2;
			glm::mat4 translate = glm::translate(physicsObject1->GetOwner()->GetComponent<Transform>()->GetPosition());
			glm::mat4 rotate = glm::mat4_cast(physicsObject1->GetOwner()->GetComponent<Transform>()->GetRotation());
			glm::mat4 scale = glm::scale(physicsObject1->GetOwner()->GetComponent<Transform>()->GetScale());
			model1 = translate * rotate * scale;

			translate = glm::translate(physicsObject2->GetOwner()->GetComponent<Transform>()->GetPosition());
			rotate = glm::mat4_cast(physicsObject2->GetOwner()->GetComponent<Transform>()->GetRotation());
			scale = glm::scale(physicsObject2->GetOwner()->GetComponent<Transform>()->GetScale());
			model2 = translate * rotate * scale;

			newContactData.LocalToWorldMatrixA = model1;
			newContactData.LocalToWorldMatrixB = model2;

			// Set to Red if colliding, Green if not colliding
			if (GJKCollisionHandler(physicsObject1, physicsObject2, newContactData))
			{
				std::cout << "Colliding!\n";
				Mesh * mesh1 = physicsObject1->GetOwner()->GetComponent<Mesh>();
				mesh1->SetVertexColorsUniform(glm::vec3(1.0f, 0.0f, 0.0f));

				Mesh * mesh2 = physicsObject2->GetOwner()->GetComponent<Mesh>();
				mesh2->SetVertexColorsUniform(glm::vec3(1.0f, 0.0f, 0.0f));

				glm::vec3 endPoint = newContactData.Position + newContactData.PenetrationDepth * newContactData.Normal;

				Line newDebugLine(glm::vec3(newContactData.Position), endPoint);
				newDebugLine.Scale = newContactData.PenetrationDepth;
				EngineHandle.GetRenderer().RegisterDebugLine(newDebugLine);

				Quad newQuad(newContactData.Position);
				EngineHandle.GetRenderer().RegisterDebugQuad(newQuad);
			}
			else
			{
				Mesh * mesh1 = physicsObject1->GetOwner()->GetComponent<Mesh>();
				mesh1->SetVertexColorsUniform(glm::vec3(0.0f, 1.0f, 0.0f));

				Mesh * mesh2 = physicsObject2->GetOwner()->GetComponent<Mesh>();
				mesh2->SetVertexColorsUniform(glm::vec3(0.0f, 1.0f, 0.0f));
			}
		}
	}
}

// Casey Muratori explains it best: https://www.youtube.com/watch?v=Qupqu1xe7Io
bool PhysicsManager::GJKCollisionHandler(Physics * aPhysicsObject1, Physics * aPhysicsObject2, ContactData & aContactData)
{
	Mesh * mesh1 = aPhysicsObject1->GetOwner()->GetComponent<Mesh>();
	Mesh * mesh2 = aPhysicsObject2->GetOwner()->GetComponent<Mesh>();

	Simplex simplex;
	// Choose initial search direction as the vector from center of Object1 to the center of Object2
	glm::vec3 searchDirection = aPhysicsObject2->GetCurrentPosition() - aPhysicsObject1->GetCurrentPosition();
	// Find farthest point along search direction to get first point on the Minkowski surface, and add it to the simplex
	SupportPoint newSupportPoint = Utility::Support(mesh1, mesh2, searchDirection, aContactData.LocalToWorldMatrixA, aContactData.LocalToWorldMatrixB);
	simplex.Push(newSupportPoint);

	// Invert the search direction for the next point
	searchDirection *= -1.0f;

	const unsigned iterationLimit = 75;
	unsigned iterationCount = 0;

	while (true)
	{
		if (iterationCount++ >= iterationLimit) return false;

		// Add a new point to the simplex, continuing to search for origin
		SupportPoint newSupportPoint = Utility::Support(mesh1, mesh2, searchDirection, aContactData.LocalToWorldMatrixA, aContactData.LocalToWorldMatrixB);
		simplex.Push(newSupportPoint);

		// If projection of newly added point along the search direction has not crossed the origin,
		// the Minkowski Difference could not contain the origin, objects are not colliding
		if (glm::dot(newSupportPoint.MinkowskiHullVertex, searchDirection) <= 0.0f)
		{
			return false;
		}
		else
		{
			// If the new point IS past the origin, check if the simplex contains the origin
			if (CheckIfSimplexContainsOrigin(simplex, searchDirection))
			{
				return EPAContactDetection(simplex, mesh1, mesh2, aContactData);
			}
		}
	}
}

// Provides the new direction to search for the point if it isn't already within the simplex
bool PhysicsManager::CheckIfSimplexContainsOrigin(Simplex & aSimplex, glm::vec3 & aSearchDirection)
{
	// Line case
	if (aSimplex.Size == 2)
	{
		// Line cannot contain the origin, only search for the direction to it 
		glm::vec3 newPointToOldPoint = aSimplex.b.MinkowskiHullVertex - aSimplex.a.MinkowskiHullVertex;
		glm::vec3 newPointToOrigin = glm::vec3(0.0f, 0.0f, 0.0f) - aSimplex.a.MinkowskiHullVertex;

		if (glm::dot(newPointToOldPoint, newPointToOrigin) > 0.0f)
		{
			// The vector from new point to old point defines the Voronoi region closest to origin
			// Get the new search direction by triple cross product, gives the normal to the edge, that points towards origin
			glm::vec3 newSearchDirection;
			newSearchDirection = Utility::TripleCrossProduct(newPointToOldPoint, newPointToOrigin, newPointToOldPoint);
			aSearchDirection = newSearchDirection;
			// No need to change the simplex, return it as [A, B]
			return false;
		}
		else
		{
			// Newly added point defines the Voronoi region closest to origin, the new point is the simplex
			aSearchDirection = newPointToOrigin;
			aSimplex.Clear();
			// Return it as [A]
			aSimplex.Push(aSimplex.a);
			return false;
		}
	}
	// Triangle case
	else if (aSimplex.Size == 3)
	{
		// Find the newly added features
		glm::vec3 newPointToOrigin = glm::vec3(0.0f, 0.0f, 0.0f) - aSimplex.a.MinkowskiHullVertex;
		glm::vec3 edge1 = aSimplex.b.MinkowskiHullVertex - aSimplex.a.MinkowskiHullVertex;
		glm::vec3 edge2 = aSimplex.c.MinkowskiHullVertex - aSimplex.a.MinkowskiHullVertex;
		// Find the normals to the triangle and the two edges
		glm::vec3 triangleNormal = glm::cross(edge1, edge2);
		glm::vec3 edge1Normal = glm::cross(edge1, triangleNormal);
		glm::vec3 edge2Normal = glm::cross(triangleNormal, edge2);

		// If origin is closer to the area along the second edge normal
		if (glm::dot(edge2Normal, newPointToOrigin) > 0.0f)
		{
			// If closer to the edge then find the normal that points towards the origin
			if (glm::dot(edge2, newPointToOrigin) > 0.0f)
			{
				aSearchDirection = Utility::TripleCrossProduct(edge2, newPointToOrigin, edge2);
				// Return it as [A, C]
				aSimplex.Clear();
				aSimplex.Set(aSimplex.a, aSimplex.c);
				return false;
			}
			// If closer to the new simplex point 
			else
			{
				// The "Star case" from the Muratori lecture
				// If new search direction should be along edge normal 
				if (glm::dot(edge1, newPointToOrigin) > 0.0f)
				{
					aSearchDirection = Utility::TripleCrossProduct(edge1, newPointToOrigin, edge1);
					// Return it as [A, B]
					aSimplex.Clear();
					aSimplex.Set(aSimplex.a, aSimplex.b);
					return false;
				}
				else // If new search direction should be along the new Simplex point
				{
					aSearchDirection = newPointToOrigin;
					// Return new simplex point alone [A]
					aSimplex.Clear();
					aSimplex.Push(aSimplex.a);
					return false;
				}
			}
		}
		else
		{
			// The "Star case" from the Muratori lecture
			// If closer to the first edge normal
			if (glm::dot(edge1Normal, newPointToOrigin) > 0.0f)
			{
				// If new search direction should be along edge normal
				if (glm::dot(edge1, newPointToOrigin) > 0.0f)
				{
					aSearchDirection = Utility::TripleCrossProduct(edge1, newPointToOrigin, edge1);
					// Return it as [A, B]
					aSimplex.Clear();
					aSimplex.Set(aSimplex.a, aSimplex.b);
					return false;
				}
				else // If new search direction should be along the new Simplex point
				{
					aSearchDirection = newPointToOrigin;
					// Return new simplex point alone [A]
					aSimplex.Clear();
					aSimplex.Push(aSimplex.a);
					return false;
				}
			}
			else
			{
				// Check if it is above the triangle
				if (glm::dot(triangleNormal, newPointToOrigin) > 0.0f)
				{
					aSearchDirection = triangleNormal;
					// No need to change the simplex, return as [A, B, C]
					return false;
				}
				else // Has to be below the triangle, all 4 other possible regions checked
				{
					aSearchDirection = -triangleNormal;
					// Return simplex as [A, C, B]
					aSimplex.Set(aSimplex.a, aSimplex.c, aSimplex.b);
					return false;
				}
			}
		}
	}
	else if (aSimplex.Size == 4)
	{
		// the simplex is a tetrahedron, must check if it is outside any of the side triangles,
		// if it is then set the simplex equal to that triangle and continue, otherwise we know
		// there is an intersection and exit

		glm::vec3 edge1 = aSimplex.b.MinkowskiHullVertex - aSimplex.a.MinkowskiHullVertex;
		glm::vec3 edge2 = aSimplex.c.MinkowskiHullVertex - aSimplex.a.MinkowskiHullVertex;
		glm::vec3 edge3 = aSimplex.d.MinkowskiHullVertex - aSimplex.a.MinkowskiHullVertex;

		glm::vec3 face1Normal = glm::cross(edge1, edge2);
		glm::vec3 face2Normal = glm::cross(edge2, edge3);
		glm::vec3 face3Normal = glm::cross(edge3, edge1);

		glm::vec3 newPointToOrigin = glm::vec3(0.0f) - aSimplex.a.MinkowskiHullVertex;

		if (glm::dot(face1Normal, newPointToOrigin) > 0.0f)
		{
			// Origin is in front of first face, simplex is correct already
			goto tag;
		}

		if (glm::dot(face2Normal, newPointToOrigin) > 0.0f)
		{
			// Origin is in front of second face, simplex is set to this triangle [A, C, D]
			aSimplex.Clear();
			aSimplex.Set(aSimplex.a, aSimplex.c, aSimplex.d);
			goto tag;
		}

		if (glm::dot(face3Normal, newPointToOrigin) > 0.0f)
		{
			// Origin is in front of third face, simplex is set to this triangle [A, D, B]
			aSimplex.Clear();
			aSimplex.Set(aSimplex.a, aSimplex.d, aSimplex.b);
			goto tag;
		}

		// If reached here it means the simplex MUST contain the origin, intersection confirmed
		return true;
	tag:
		// the simplex is equal to the triangle that the origin is in front of
		// this is exactly the same as the triangular simplex test except that we know
		// that the origin is not behind the triangle

		glm::vec3 edge1Normal = glm::cross(edge1, face1Normal);
		if (glm::dot(edge1Normal, newPointToOrigin) > 0.0f)
		{
			aSearchDirection = Utility::TripleCrossProduct(edge1, newPointToOrigin, edge1);
			// Origin is along the normal of edge1, set the simplex to that edge [A, B]
			aSimplex.Clear();
			aSimplex.Set(aSimplex.a, aSimplex.b);
			return false;
		}

		glm::vec3 edge2Normal = glm::cross(face1Normal, edge2);
		if (glm::dot(edge1Normal, newPointToOrigin) > 0.0f)
		{
			aSearchDirection = Utility::TripleCrossProduct(edge2, newPointToOrigin, edge2);
			// Origin is along the normal of edge2, set the simplex to that edge [A, C]
			aSimplex.Clear();
			aSimplex.Set(aSimplex.a, aSimplex.c);
			return false;
		}

		aSearchDirection = face1Normal;
		// If reached here the origin is along the first face normal, set the simplex to this face [A, B, C]
		aSimplex.Clear();
		aSimplex.Set(aSimplex.a, aSimplex.b, aSimplex.c);
		return false;
	}
}

// Based on the Expanding Polytope Algorithm (EPA) as described here: http://allenchou.net/2013/12/game-physics-contact-generation-epa/
bool PhysicsManager::EPAContactDetection(Simplex & aSimplex, Primitive * aShape1, Primitive * aShape2, ContactData & aContactData)
{
	const float exitThreshold = 0.001f;
	const unsigned iterationLimit = 50;
	unsigned iterationCount = 0;

	std::list<PolytopeFace> polytopeFaces;
	std::list<PolytopeEdge> polytopeEdges;

	// Add all faces of simplex to the polytope
	polytopeFaces.emplace_back(aSimplex.a, aSimplex.b, aSimplex.c);
	polytopeFaces.emplace_back(aSimplex.a, aSimplex.c, aSimplex.d);
	polytopeFaces.emplace_back(aSimplex.a, aSimplex.d, aSimplex.b);
	polytopeFaces.emplace_back(aSimplex.b, aSimplex.d, aSimplex.c);
	while (true)
	{
		if (iterationCount++ >= iterationLimit) return false;

		// Find the closest face to origin (i.e. projection of any vertex along its face normal with the least value)
		float minimumDistance = FLT_MAX;
		std::list<PolytopeFace>::iterator closestFace = polytopeFaces.begin();
		for (auto iterator = polytopeFaces.begin(); iterator != polytopeFaces.end(); ++iterator)
		{
			float distance = glm::dot(iterator->FaceNormal, iterator->Points[0].MinkowskiHullVertex);
			if (distance < minimumDistance)
			{
				minimumDistance = distance;
				closestFace = iterator;
			}
		}
		// With the closest face now known, find new support point on the Minkowski Hull using normal to that face
		SupportPoint newPolytopePoint = Utility::Support(aShape1, aShape2, closestFace->FaceNormal, aContactData.LocalToWorldMatrixA, aContactData.LocalToWorldMatrixB);

		// If this new point is within a tolerable limit of the origin, 
		// assume we have found the closest triangle on the Minkowski Hull to the origin
		if (glm::dot(closestFace->FaceNormal, closestFace->Points[0].MinkowskiHullVertex) - minimumDistance < exitThreshold)
		{
			return ExtrapolateContactInformation(&(*closestFace), aContactData);
		}

		// Otherwise, check what faces can be 'seen' from the newly added support point and delete them from the polytope
		for (auto iterator = polytopeFaces.begin(); iterator != polytopeFaces.end();)
		{
			// A face is considered as "seen" if the new support point is on the positive halfspace of the plane defined by it
			glm::vec3 planeVector = newPolytopePoint.MinkowskiHullVertex - iterator->Points[0].MinkowskiHullVertex;

			if (glm::dot(iterator->FaceNormal, planeVector) > 0.0f)
			{
				// Only adds the outside edges of the 'seen' face, and ignores the others
				Utility::AddEdge(polytopeEdges, iterator->Points[0], iterator->Points[1]);
				Utility::AddEdge(polytopeEdges, iterator->Points[1], iterator->Points[2]);
				Utility::AddEdge(polytopeEdges, iterator->Points[2], iterator->Points[0]);
				// Remove the face from the Polytope
				iterator = polytopeFaces.erase(iterator);
				continue;
			}
			++iterator;
		}

		// Create new polytope faces using the new support point from the valid edges in the edge list
		for (auto iterator = polytopeEdges.begin(); iterator != polytopeEdges.end(); ++iterator)
		{
			polytopeFaces.emplace_back(newPolytopePoint, iterator->Points[0], iterator->Points[1]);
		}

		// Clear the edge list every iteration of the expansion
		polytopeEdges.clear();
	}
}

// By using the closest face of the Minkowski hull to the origin in Minkowski space, 
// you obtain the closest face to the penetration in world space.
// Barycentric projection allows you to save the contribution to a point 
// in a triangle between its vertices, and you can use that to obtain the contact point in world space
bool PhysicsManager::ExtrapolateContactInformation(PolytopeFace * aClosestFace, ContactData & aContactData)
{
	const float distanceFromOrigin = glm::dot(aClosestFace->FaceNormal, aClosestFace->Points[0].MinkowskiHullVertex);

	// calculate the barycentric coordinates of the closest triangle with respect to
	// the projection of the origin onto the triangle
	float bary_u, bary_v, bary_w;
	Utility::BarycentricProjection(aClosestFace->FaceNormal * distanceFromOrigin, aClosestFace->Points[0].MinkowskiHullVertex, aClosestFace->Points[1].MinkowskiHullVertex, aClosestFace->Points[2].MinkowskiHullVertex, bary_u, bary_v, bary_w);
	
	// if any of the barycentric coefficients have a magnitude greater than 1, then the origin is not within the triangular prism described by 'triangle'
	// thus, there is no collision here, return false
	if (fabs(bary_u) > 1.0f || fabs(bary_v) > 1.0f || fabs(bary_w) > 1.0f)
		return false;

	glm::vec3 supportWorld1 = glm::vec3(glm::vec4(aClosestFace->Points[0].Local_SupportPointA, 1) * aContactData.LocalToWorldMatrixA);
	glm::vec3 supportWorld2 = glm::vec3(glm::vec4(aClosestFace->Points[1].Local_SupportPointA, 1) * aContactData.LocalToWorldMatrixA);
	glm::vec3 supportWorld3 = glm::vec3(glm::vec4(aClosestFace->Points[2].Local_SupportPointA, 1) * aContactData.LocalToWorldMatrixA);

	// Contact point on object A in local space
	aContactData.Position = (bary_u * supportWorld1) + (bary_v * supportWorld2) + (bary_w * supportWorld3);
	// Contact normal
	aContactData.Normal = glm::normalize(-aClosestFace->FaceNormal);
	// Penetration depth
	aContactData.PenetrationDepth = distanceFromOrigin;
	
	return true;
}



void PhysicsManager::OnNotify(Object * object, Event * event)
{
	// Check if this is an Engine event
	EngineEvent * engineEvent = nullptr;
	engineEvent = dynamic_cast<EngineEvent *>(event);

	if (engineEvent)
	{
		switch (engineEvent->EventID)
		{
			case EngineEvent::EventList::ENGINE_INIT:
			{
			}
			case EngineEvent::EventList::ENGINE_TICK:
			{
				Update();
			}
		return;
		}
	}

}

void PhysicsManager::RegisterComponent(Physics * aNewPhysics)
{
	PhysicsObjectsList.push_back(aNewPhysics);
}

void PhysicsManager::Simulation()
{
	Physics * pSimulation1 = nullptr, * pSimulation2 = nullptr;
	float deltatime = EngineHandle.GetFramerateController().DeltaTime;

	// Integration
	for (int i = 0; i < Iterations; ++i)
	{
		for (auto iterator = PhysicsObjectsList.begin(); iterator != PhysicsObjectsList.end(); ++iterator)
		{
			pSimulation1 = static_cast<Physics *>(*iterator);
			// Updates physics component with current transform values
			pSimulation1->SyncPhysicsWithTransform();

			if (EngineHandle.GetInputManager().isKeyPressed(GLFW_KEY_1))
			{
				pSimulation1->IntegratePositionVerlet(deltatime);
			}
			else if (EngineHandle.GetInputManager().isKeyPressed(GLFW_KEY_2))
			{
				pSimulation1->IntegrateRK4(EngineHandle.GetFramerateController().TotalTime, deltatime);
			}
			else
			{
				// Default type of integration is Euler
				pSimulation1->IntegrateExplicitEuler(deltatime);
			}
		}
	}
}
