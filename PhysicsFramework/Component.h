#pragma once
#include <iostream>
#include "ResourceManager.h"
#include "Observer.h"
class GameObject;
class Event;

// Component is a special type of Object that should be used for easily swappable behavior
// Components are not meant to be exist in the 'world' like Game objects do
// They only exist as attachments to an already extant Game object

class Component : public Object	// Pure virtual class 
{
	/*----------MEMBER VARIABLES----------*/
public:
	enum ComponentType
	{
		TRANSFORM,
		PRIMITIVE,
		PHYSICS,
		TypeCount
	};
private:
	GameObject * pOwner;
	ComponentType eComponentType;

	/*----------MEMBER FUNCTIONS----------*/
private:
	virtual void Serialize(TextFileData aTextData) = 0;
	virtual void HandleEvent(Event *pEvent) {}
protected:	// Made protected because we don't want objects of this class to be instantiated directly, but derived classes can access it
	Component(ComponentType type) : eComponentType(type) {}
public:
	virtual ~Component() {}
	static inline ComponentType GetComponentID() { return ComponentType::TypeCount; };

	inline ComponentType GetComponentType() { return eComponentType; }
	inline GameObject * GetOwner() { return pOwner; }
	inline GameObject const * GetConstOwner() { return pOwner; }
	inline void SetOwner(GameObject * aOwner) { pOwner = aOwner; }

	virtual void Update() = 0;
};