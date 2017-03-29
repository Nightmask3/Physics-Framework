#pragma once
#include "Observer.h"
#include "GameObject.h"
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
class Physics;
class Collider;

class PhysicsManager : public Observer
{
	/*----------MEMBER VARIABLES----------*/
public:
	static int Iterations;
	/*---ENGINE REFERENCE ---*/
	Engine & EngineHandle;

	std::vector<Physics *> PhysicsObjectsList;
	std::vector<Collider *> ColliderObjectsList;
	/*----------MEMBER FUNCTIONS----------*/
	PhysicsManager(Engine & aEngine) :EngineHandle(aEngine) {};
	~PhysicsManager() {};

	Engine const & GetEngine() { return EngineHandle; }

	void RegisterPhysicsObject(Physics * aNewPhysics);
	void RegisterColliderObject(Collider * aNewCollider);

	void Simulation();
	void Update();

	void DetectCollision();
	bool GJKCollisionHandler(Collider * aCollider1, Collider * aCollider2, ContactData & aContactData);
	bool EPAContactDetection(Simplex & aSimplex, Collider * aShape1, Collider * aShape2, ContactData & aContactData);
	bool ExtrapolateContactInformation(PolytopeFace * aClosestFace, ContactData & aContactData);
	bool CheckIfSimplexContainsOrigin(Simplex & aSimplex, glm::vec3 & aSearchDirection);

	virtual void OnNotify(Event * aEvent) override;

};