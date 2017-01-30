#include <glm/glm.hpp>

#include "InputManager.h"
#include "PhysicsManager.h"
#include "FrameRateController.h"
#include "Transform.h"
const float G = 0.000066742f;

int PhysicsManager::Iterations = 10;

void PhysicsManager::Update()
{
	if (InputManagerReference.isKeyPressed(GLFW_KEY_KP_ADD) && Iterations <= 200)
		std::cout<<"New Iteration Count:" << ++Iterations << std::endl;
	else if(InputManagerReference.isKeyPressed(GLFW_KEY_KP_SUBTRACT) && Iterations >= 1)
		std::cout << "New Iteration Count:" << --Iterations << std::endl;

	// Three Stages
	// Simulation : Update the physics variables of all objects that have physics components (Apply gravity, velocity, forces etc.)
	Simulation();

	// Collision Detection : Checking every Physics Component that has a Rigid Body for collision against every other rigid body
	//DetectCollision();
	// Resolution: Apply the physics values to the game object transforms
	//Resolution();
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
	Transform * transform = static_cast<Transform *>(aNewPhysics->GetOwner()->GetComponent(Component::TRANSFORM));
	aNewPhysics->SetPosition(transform->GetPosition());
	aNewPhysics->SetPositionNext(transform->GetPosition());
	PhysicsObjectsList.push_back(aNewPhysics);
}

void PhysicsManager::Simulation()
{
	Physics * pSimulation1 = nullptr, * pSimulation2 = nullptr;
	float deltatime = FrameManagerReference.GetDeltaTime();

	//// Gravitational Force Simulation
	///*for (int i = 0; i < PhysicsObjectsList.size(); ++i)
	//{*/
	//	pSimulation1 = PhysicsObjectsList[0];
	//	for (int j = 1; j < PhysicsObjectsList.size(); ++j)
	//	{
	//		/*if (i != j)
	//		{*/
	//			pSimulation2 = PhysicsObjectsList[j];

	//			float numerator = G * pSimulation1->GetMass() * pSimulation2->GetMass();
	//			glm::vec3 distanceVector = pSimulation2->GetPositionCurr() - pSimulation1->GetPositionCurr();
	//			float denominator = pow(glm::length(distanceVector), 2) + 0.1f;
	//			float value = numerator / denominator;
	//			glm::vec3 gravForce = value * distanceVector;

	//			pSimulation2->ApplyForce(-gravForce);
	//			//pSimulation1->ApplyForce(gravForce);
	//		/*}*/
	//	}
	///*}*/


	// Integration
	for (int i = 0; i < Iterations; ++i)
	{
		for (auto iterator = PhysicsObjectsList.begin(); iterator != PhysicsObjectsList.end(); ++iterator)
		{
			pSimulation1 = static_cast<Physics *>(*iterator);

			if (InputManagerReference.isKeyPressed(GLFW_KEY_1))
			{
				pSimulation1->IntegratePositionVerlet(deltatime);
			}
			else if (InputManagerReference.isKeyPressed(GLFW_KEY_2))
			{
				pSimulation1->IntegrateRK4(FrameManagerReference.GetTotalTime(), deltatime);
			}
			else
			{
				// Default type of integration is Euler
				pSimulation1->IntegrateExplicitEuler(deltatime);
			}
		}
	}
}
