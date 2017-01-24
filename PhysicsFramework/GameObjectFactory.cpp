#include <cstdio>

#include "GameObjectFactory.h"
GameObject * GameObjectFactory::SpawnGameObjectFromArchetype(const char * aFileName)
{
	char * archetypeContents = ResourceManagerReference.LoadTextFile(aFileName, READ);
	unsigned int counter = 0;
	unsigned char tempC = '\0'; // Used to hold temporary character values for component names
	std::string componentName;
	while (archetypeContents[counter] != '\0')		// While not end of file
	{

		componentName = std::string(gets(archetypeContents));
		while ((contents.at(counter) != '\r')) // Reads character by character until new line
		{
			tempC = contents.at(counter++);
			componentName += tempC;			 // Gets type of component to create
		}
		if (strcmp(componentName.c_str(), "Sprite Component") == 0)	// Evaluates to 0 if it is true
		{
			// SET THE COMPONENT VALUES HERE FROM THE FILE
			SpriteComponent * s = nullptr;
			CreateComponent<SpriteComponent>();
			s = static_cast<SpriteComponent *>(ObjectLibrary.back().GetComponent(Component::ComponentType::SPRITE));
			s->Serialize(contents, counter);
		}
		else if (strcmp(componentName.c_str(), "Transform Component") == 0)
		{
			// SET THE TRANSFORM COMPONENT VALUES HERE FROM THE FILE
			TransformComponent * t = nullptr;
			CreateComponent<TransformComponent>();
			t = static_cast<TransformComponent *>(ObjectLibrary.back().GetComponent(Component::ComponentType::TRANSFORM));
			t->Serialize(contents, counter);
		}
		else if (strcmp(componentName.c_str(), "Physics Component") == 0)
		{
			// SET THE COMPONENT VALUES HERE FROM THE FILE
			PhysicsComponent * p = nullptr;
			CreateComponent<PhysicsComponent>();
			p = static_cast<PhysicsComponent *>(ObjectLibrary.back().GetComponent(Component::ComponentType::PHYSICS));
			p->Serialize(contents, counter);
		}
		else if (strcmp(componentName.c_str(), "Keyboard Controller Component") == 0)
		{
			// SET THE COMPONENT VALUES HERE FROM THE FILE
			KeyboardControllerComponent * k = nullptr;
			CreateComponent<KeyboardControllerComponent>();
			k = static_cast<KeyboardControllerComponent *>(ObjectLibrary.back().GetComponent(Component::ComponentType::KBCONTROLLER));
		}
		counter += 3; // To skip newline and carriage return characters
		if (counter > contents.size())
			return;
	}
}

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
