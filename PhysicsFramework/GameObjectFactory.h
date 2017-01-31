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

class GameObjectFactory : public Observer
{
	/*----------MEMBER VARIABLES----------*/
private:
	std::vector<std::unique_ptr<GameObject>> GameObjectList;
	Engine * pEngineReference;
	/*----------MEMBER FUNCTIONS----------*/
public:
	GameObjectFactory(Engine * aEngine) : pEngineReference(aEngine)
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
			int renderListSize = pEngineReference->GetRenderer().GetRenderListSize();
			// adding 1 because OpenGL regards VAO and VBO value of 0 to be unset (great choice there -.-)
			mComponent = new Sprite(renderListSize + 1, renderListSize + 1);
			mComponent->SetOwner(aOwner);
			pEngineReference->GetRenderer().RegisterPrimitive(static_cast<Primitive *>(mComponent));
		}
		else if (typeid(T) == typeid(Mesh))
		{
			int renderListSize = pEngineReference->GetRenderer().GetRenderListSize();
			mComponent = new Mesh(renderListSize + 1, renderListSize + 1);
			mComponent->SetOwner(aOwner);
			pEngineReference->GetRenderer().RegisterPrimitive(static_cast<Primitive *>(mComponent));
		}
		else if (typeid(T) == typeid(Physics))
		{
			mComponent = new Physics();
			mComponent->SetOwner(aOwner);
			pEngineReference->GetPhysicsManager().RegisterComponent(static_cast<Physics *>(mComponent));
		}
		return static_cast<T *>(mComponent);
	}
	
	virtual void OnNotify(Object * object, Event * event) override;

};
