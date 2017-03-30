#pragma once
// Abstract base class for any type of constraint
// A constraint is used to affect/limit an objects degree of freedom(s)
class Collider;
class Constraint
{
	/*-----------MEMBER VARIABLES-----------*/
public:
	Collider * ColliderA;
	Collider * ColliderB;

	/*-----------MEMBER FUNCTIONS-----------*/
public:
	Constraint(Collider & aColliderA, Collider & aColliderB) :
		ColliderA(&aColliderA),
		ColliderB(&aColliderB) {}
	// All constraints must implement how the solver will handle them
	virtual void Solve() = 0;
	virtual void CalculateJacobian() = 0;

};