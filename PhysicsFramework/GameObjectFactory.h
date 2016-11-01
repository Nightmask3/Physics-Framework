#pragma once
#include <sstream>
#include <vector>
#include <typeinfo>
#include <typeindex>
// MANAGERS
#include "Renderer.h"
#include "InputManager.h"
#include "FrameRateController.h"
#include "PhysicsManager.h"

#include "Observer.h"
// GAME OBJECT
#include "GameObject.h"
// COMPONENTS
#include "Transform.h"
#include "Physics.h"

class Engine;
class Renderer;

class GameObjectFactory : public Observer
{
	/*----------MEMBER VARIABLES----------*/
private:
	std::vector<std::unique_ptr<GameObject>> GameObjectList;
	// Needs to be a pointer as we need to modify event list
	Engine * pEngineReference;
	Renderer & RendererReference;
	PhysicsManager & PhysicsManagerReference;
	/*----------MEMBER FUNCTIONS----------*/
public:
	GameObjectFactory(Engine * aEngine, Renderer & render, PhysicsManager & phy) : pEngineReference(aEngine), RendererReference(render), PhysicsManagerReference(phy) { GameObjectList.reserve(4096); }
	virtual ~GameObjectFactory() {};

	GameObject * SpawnGameObject(Transform & aTransform = Transform());
	template <typename T> T * SpawnComponent(GameObject * aOwner)
	{
		Component * mComponent = nullptr;
		if (typeid(T) == typeid(Sprite))
		{
			mComponent = new Sprite(RendererReference.GetRenderListSize() + 1, RendererReference.GetRenderListSize() + 1);
			mComponent->SetOwner(aOwner);
			RendererReference.RegisterPrimitive(static_cast<Primitive *>(mComponent));
		}
		else if (typeid(T) == typeid(Physics))
		{
			mComponent = new Physics();
			mComponent->SetOwner(aOwner);
			PhysicsManagerReference.RegisterComponent(static_cast<Physics *>(mComponent));
		}
		return static_cast<T *>(mComponent);
	}
	
	virtual void OnNotify(Object * object, Event * event) override;

};
