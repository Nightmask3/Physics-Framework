#pragma once
#include "Component.h"
#include "GameObject.h"
#include "ScriptBehavior.h"

class Script : public Component
{
	/*-----------MEMBER VARIABLES-----------*/
public:
	// Every script has a pointer to a 'ScriptBehavior' 
	// which defines a 'Behavior' function that takes as an argument the calling Script and returns nothing
	ScriptBehavior * pScriptBehavior;

	/*-----------MEMBER FUNCTIONS-----------*/
public:
	Script() : Component(Component::SCRIPT) {}
	~Script() { delete pScriptBehavior; }

	virtual void Deserialize(TextFileData aTextData) override {};
	
	inline void SetBehavior(ScriptBehavior * aScriptBehavior) {
		pScriptBehavior = aScriptBehavior;
	}

	// Calling the script is just a wrapper around the behavior
	inline void CallScript() {
		if (pScriptBehavior != nullptr)
			pScriptBehavior->Behavior(this);
		else
		{
			std::cout << "CallScript() : Script Belonging to: " << pOwner->Name << " does not have any behavior bound to it!";
		}
	}
	virtual void Update() override { CallScript(); };
};
