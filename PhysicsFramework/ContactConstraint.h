#pragma once
// Eigen arbitrary size dense matrix header
#include <Eigen/Dense>
#include "Constraint.h"
#include "PhysicsUtilities.h"

// Constraint used to prevent two objects from geometric interference (collision)
// http://allenchou.net/2013/12/game-physics-resolution-contact-constraints/
class ContactConstraint : public Constraint
{
	/*-----------MEMBER VARIABLES-----------*/
public:
	// The contact Jacobian is a 12-by-1 row vector
	Eigen::Matrix<float, 1, 12> Jacobian;
	ContactData ConstraintData;
	// Lagrangian multiplier/ force of impulse to resolve the constraint
	float ImpulseMagnitude;
	/*-----------MEMBER FUNCTIONS-----------*/
public:
	ContactConstraint(Collider & aColliderA, Collider & aColliderB) : Constraint(aColliderA, aColliderB)
	{
		// Calculate Jacobian when created
		CalculateJacobian();
	}
	virtual void CalculateJacobian() override;
	virtual void Solve() override;

};