#pragma once
#include "Observer.h"
#include "GameObject.h"
#include "Physics.h"

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
private:
	static int Iterations;
	/*------------------------------- ENGINE REFERENCE -------------------------------*/
	Engine const & EngineHandle;

public:
	std::vector<Physics *> PhysicsObjectsList;
	
	/*----------MEMBER FUNCTIONS----------*/
	PhysicsManager(Engine const & aEngine) :EngineHandle(aEngine) {};
	~PhysicsManager() {};

	Engine const & GetEngine() { return EngineHandle; }

	void RegisterComponent(Physics * aNewPhysics);

	void Simulation();
	void Update();
	
	virtual void OnNotify(Object * object, Event * event) override;
};