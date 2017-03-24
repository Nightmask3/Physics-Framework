#pragma once
#include "Component.h"
class Script;

struct ScriptBehavior 
{
	/*-----------MEMBER VARIABLES-----------*/
public:
	// A list of what component types are necessary in order to run this script behavior
	std::vector<Component::ComponentType> ComponentPrerequisites;
	~ScriptBehavior() {}
	/*-----------MEMBER FUNCTIONS-----------*/
public:
	virtual void Behavior(Script * aOwningScript) = 0 { std::cout << "Base Behavior!\n"; }
};