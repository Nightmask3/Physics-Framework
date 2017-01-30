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
#include "Sprite.h"
#include "Mesh.h"

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
	ResourceManager const & ResourceManagerReference;
	/*----------MEMBER FUNCTIONS----------*/
public:
	GameObjectFactory(Engine * aEngine, Renderer & render, PhysicsManager & phy, ResourceManager const & resource) : 
		pEngineReference(aEngine), 
		RendererReference(render), 
		PhysicsManagerReference(phy),
		ResourceManagerReference(resource)
	{ GameObjectList.reserve(4096); }
	virtual ~GameObjectFactory() {};

	GameObject * SpawnGameObjectFromArchetype(const char * aFileName);
	GameObject * SpawnGameObject(Transform & aTransform = Transform());
	template <typename T> T * SpawnComponent(GameObject * aOwner)
	{
		Component * mComponent = nullptr;
		if (typeid(T) == typeid(Transform))
		{
			// Create root component from supplied/default transform
			mComponent = new Transform();
			mComponent->SetOwner(aOwner);	
		}
		if (typeid(T) == typeid(Sprite))
		{
			// adding 1 because OpenGL regards VAO and VBO value of 0 to be unset (great choice there -.-)
			mComponent = new Sprite(RendererReference.GetRenderListSize() + 1, RendererReference.GetRenderListSize() + 1);
			mComponent->SetOwner(aOwner);
			RendererReference.RegisterPrimitive(static_cast<Primitive *>(mComponent));
		}
		else if (typeid(T) == typeid(Mesh))
		{
			mComponent = new Mesh(RendererReference.GetRenderListSize() + 1, RendererReference.GetRenderListSize() + 1);
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
