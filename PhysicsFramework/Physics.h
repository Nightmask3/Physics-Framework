#pragma once
#include <glm/vec3.hpp>
#include "Component.h"

class Physics : public Component
{
public:
	/* -------- VARIABLES ---------- */
	struct Derivative
	{
		glm::vec3 mDerivedVelocity;
		glm::vec3 mDerivedAcceleration;
		Derivative()
		{
			mDerivedAcceleration= glm::vec3();
			mDerivedVelocity = glm::vec3();
		}
	};
	glm::vec3 PositionNext;
	glm::vec3 PositionCurr;
	glm::vec3 PositionPrev;
	glm::vec3 Velocity;
	glm::vec3 Force;
	glm::vec3 Momentum;
	float Mass;
	float InverseMass;
	/* -------- FUNCTIONS ---------- */
	
	Physics() : Component(Component::PHYSICS)
	{
		Velocity = glm::vec3();
		PositionNext = glm::vec3();
		PositionCurr = glm::vec3();
		PositionPrev = glm::vec3();
		Momentum = glm::vec3();
		Force = glm::vec3();
		Mass = 100;
		InverseMass = 1.0f;
	}
	~Physics() {}
	
	
	// GETTERS
	inline float GetMass() { return Mass; }
	inline glm::vec3 GetPositionCurr() { return PositionCurr; }
	inline glm::vec3 GetVelocity() { return Velocity; }
	// SETTERS
	inline void SetMass(float mass) { Mass = mass; InverseMass = 1 / Mass; }
	inline void SetPosition(glm::vec3 position) { PositionCurr = position; }
	inline void SetPositionNext(glm::vec3 position) { PositionNext = position; }
	inline void ApplyForce(glm::vec3 newForce) { Force += newForce; }

	void Update() {}
	void Serialize(std::string & Contents, unsigned int & Count) {};
	
	void UpdateTransform();
	void Recalculate();
	Derivative Evaluate(float t, float dt, const Derivative &);
	void IntegrateExplicitEuler(float dt);
	void IntegrateRK4(float totalTime, float dt);
	void IntegratePositionVerlet(float dt);
};