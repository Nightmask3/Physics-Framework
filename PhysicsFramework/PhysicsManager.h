#pragma once
#include "Observer.h"
#include "GameObject.h"
#include "Physics.h"
#include "Primitive.h"
#include "PhysicsUtilities.h"

class CollideEvent : public Event
{
public:
	GameObject * mpObject1;
	GameObject * mpObject2;

	CollideEvent();// : Event(COLLIDE) { mpObject1 = nullptr; mpObject2 = nullptr; }
	virtual ~CollideEvent() {}
};

class FramerateController;
class InputManager;

class PhysicsManager : public Observer
{
	/*----------MEMBER VARIABLES----------*/
public:
	static int Iterations;
	/*---ENGINE REFERENCE ---*/
	Engine & EngineHandle;

	std::vector<Physics *> PhysicsObjectsList;
	
	/*----------MEMBER FUNCTIONS----------*/
	PhysicsManager(Engine & aEngine) :EngineHandle(aEngine) {};
	~PhysicsManager() {};

	Engine const & GetEngine() { return EngineHandle; }

	void RegisterComponent(Physics * aNewPhysics);

	void Simulation();
	void Update();

	void DetectCollision();
	bool GJKCollisionHandler(Physics * aPhysicsObject1, Physics * aPhysicsObject2, ContactData & aContactData);
	bool EPAContactDetection(Simplex & aSimplex, Primitive * aShape1, Primitive * aShape2, ContactData & aContactData);
	bool ExtrapolateContactInformation(PolytopeFace * aClosestFace, ContactData & aContactData);
	bool CheckIfSimplexContainsOrigin(Simplex & aSimplex, glm::vec3 & aSearchDirection);

	virtual void OnNotify(Event * aEvent) override;

};