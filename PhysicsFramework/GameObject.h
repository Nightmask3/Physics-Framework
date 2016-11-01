#pragma once
#include <vector>
#include <memory>

#include "Object.h"
#include "Component.h"
#include "Event.h"

class GameObject : public Observer
{
	/*----------MEMBER VARIABLES----------*/
private:
	std::vector<std::unique_ptr<Component>> ComponentList;	

	/*----------MEMBER FUNCTIONS----------*/
public:
	GameObject() {}
	virtual ~GameObject() {}
	
	Component * GetComponent(Component::ComponentType);
	Component * GetComponent(Component::ComponentType) const;
	std::vector<std::unique_ptr<Component>> const & GetComponentList() { return ComponentList; }
	
	void Update();
	void AddComponent(Component * aNewComponent);
	void HandleEvent(Event *);
private:
	virtual void OnNotify(Object * object, Event * event) override;
};
