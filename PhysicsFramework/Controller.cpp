#include "Controller.h"
#include "Physics.h"
#include "GameObject.h"

Component::ComponentType Controller::GetComponentID()
{
	return Component::ComponentType::CONTROLLER;
}

void Controller::Update()
{
	if (InputManagerReference.isKeyPressed(GLFW_KEY_UP))
	{
		TargetTransform->Position += upVector * MovementSpeed * FrameRateControllerReference.DeltaTime;
	}
	if (InputManagerReference.isKeyPressed(GLFW_KEY_DOWN))
	{
		TargetTransform->Position += upVector * (-MovementSpeed) * FrameRateControllerReference.DeltaTime;
	}
	if (InputManagerReference.isKeyPressed(GLFW_KEY_LEFT))
	{
		TargetTransform->Position += leftVector * MovementSpeed * FrameRateControllerReference.DeltaTime;
	}
	if (InputManagerReference.isKeyPressed(GLFW_KEY_RIGHT))
	{
		TargetTransform->Position += leftVector * (-MovementSpeed) * FrameRateControllerReference.DeltaTime;
	}

	// If owner has a physics component, update it directly along with the transform
	Physics * physics = nullptr;
	physics = GetOwner()->GetComponent<Physics>();
	if (physics)
	{
		physics->SetPosition(TargetTransform->Position);
	}
}
