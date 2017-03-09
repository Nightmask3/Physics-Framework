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
public:
	std::string Name;
	/*----------MEMBER FUNCTIONS----------*/
public:
	GameObject() {}
	virtual ~GameObject() {}
	
	template <typename T> T * GetComponent()
	{
		return dynamic_cast<T *>(GetComponent(T::GetComponentID()));
	}
	template <typename T> T * GetComponent() const
	{
		return dynamic_cast<T *>(GetComponent(T::GetComponentID()));
	}

	std::vector<std::unique_ptr<Component>> const & GetComponentList() { return ComponentList; }
	
	void Update();
	void AddComponent(Component * aNewComponent);

private:
	virtual void OnNotify(Object * object, Event * event) override;

	Component * GetComponent(Component::ComponentType);
	Component * GetComponent(Component::ComponentType) const;
};
