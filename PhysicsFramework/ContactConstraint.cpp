#include "ContactConstraint.h"
#include "PhysicsUtilities.h"
#include "GameObject.h"
#include "Transform.h"
#include "Physics.h"
#include "Collider.h"

void ContactConstraint::CalculateJacobian()
{
	glm::vec3 & contactNormal = ConstraintData.Normal;
	
	glm::vec3 & centerOfMassA = ColliderA->pOwner->GetComponent<Transform>()->GetPosition();
	glm::vec3 momentArmA = ConstraintData.ColliderData.World_SupportPointA - centerOfMassA;
	
	glm::vec3 & centerOfMassB = ColliderB->pOwner->GetComponent<Transform>()->GetPosition();
	glm::vec3 momentArmB = ConstraintData.ColliderData.World_SupportPointB - centerOfMassB;

	glm::vec3 crossProductA = glm::cross(-momentArmA, contactNormal);
	glm::vec3 crossProductB = glm::cross(momentArmB, contactNormal);

	// Calculates Jacobian for the contact constraint
	Jacobian << -contactNormal.x, -contactNormal.y, -contactNormal.z,
				 crossProductA.x, crossProductA.y, crossProductA.z,
				 contactNormal.x, contactNormal.y, contactNormal.z,
				 crossProductB.x, crossProductB.y, crossProductB.z;

}

void ContactConstraint::Solve()
{
	Physics & physicsA = *ColliderA->pOwner->GetComponent<Physics>();
	Physics & physicsB = *ColliderB->pOwner->GetComponent<Physics>();

	// Calculate inverse mass matrix for entire constraint system
	Eigen::MatrixXf inverseMassMatrix(12, 12);

	// Calculate inverse mass matrix of object A
	Eigen::Matrix3f inverseMassMatrixA;
	inverseMassMatrixA.setIdentity();
	inverseMassMatrixA *= physicsA.InverseMass;

	// Calculate inverse mass matrix of object B
	Eigen::Matrix3f inverseMassMatrixB;
	inverseMassMatrixB.setIdentity();
	inverseMassMatrixB *= physicsB.InverseMass;

	// Convert from GLM matrices to Eigen matrices
	Eigen::Matrix3f inverseInertiaTensorA;
	Eigen::Matrix3f inverseInertiaTensorB;

	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			inverseInertiaTensorA(i, j) = physicsA.InverseInertiaTensor[i][j];
			inverseInertiaTensorB(i, j) = physicsB.InverseInertiaTensor[i][j];
		}
	}

	inverseMassMatrix << inverseMassMatrixA, Eigen::Matrix3f::Zero(), Eigen::Matrix3f::Zero(), Eigen::Matrix3f::Zero(),
						 Eigen::Matrix3f::Zero(), inverseInertiaTensorA, Eigen::Matrix3f::Zero(), Eigen::Matrix3f::Zero(),
						 Eigen::Matrix3f::Zero(), Eigen::Matrix3f::Zero(), inverseMassMatrixB, Eigen::Matrix3f::Zero(),
						 Eigen::Matrix3f::Zero(), Eigen::Matrix3f::Zero(), Eigen::Matrix3f::Zero(), inverseInertiaTensorB;
	
	// Column vector obtained from multiplying Inverse Mass Matrix and Jacobian Transpose
	Eigen::Matrix<float, 12, 1> bMatrix;
	bMatrix = inverseMassMatrix * Jacobian.transpose();

	float effectiveMass = Jacobian * bMatrix;
}
