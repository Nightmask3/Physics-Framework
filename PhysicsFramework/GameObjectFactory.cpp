#include "GameObjectFactory.h"

GameObject * GameObjectFactory::SpawnGameObject(Transform & aTransform)
{
	GameObject * newGameObject = new GameObject();
	// Create root component from supplied/default transform
	Transform * rootComponent = new Transform(aTransform);
	rootComponent->SetOwner(newGameObject);
	// Add component pointer to newly created game object
	newGameObject->AddComponent(rootComponent);
	// Add game object pointer to list
	GameObjectList.emplace_back(newGameObject);	

	// Add game object to observer list of engine tick event
	pEngineReference->GetMainEventList()[EngineEvent::ENGINE_TICK].AddObserver(newGameObject);
	
	return newGameObject;
}

void GameObjectFactory::OnNotify(Object * object, Event * event)
{
	// Check if this is an Engine event
	EngineEvent * engineEvent = static_cast<EngineEvent *>(event);
	if (engineEvent)
	{
		switch (engineEvent->EventID)
		{
			case EngineEvent::EventList::ENGINE_INIT:
			{
				
			}
		}
	}
}
