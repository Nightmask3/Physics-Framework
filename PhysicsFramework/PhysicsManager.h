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

class FrameRateController;
class InputManager;

class PhysicsManager : public Observer
{
	/*----------MEMBER VARIABLES----------*/
private:
	static int Iterations;
	FrameRateController & FrameManagerReference;
	InputManager & InputManagerReference;
public:
	std::vector<Physics *> PhysicsObjectsList;
	
	/*----------MEMBER FUNCTIONS----------*/
	PhysicsManager(FrameRateController & frc, InputManager & in) : FrameManagerReference(frc), InputManagerReference(in) {}
	~PhysicsManager() {};

	void RegisterComponent(Physics * aNewPhysics);

	void Simulation();
	void Update();
	
	virtual void OnNotify(Object * object, Event * event) override;
};