#pragma once
#include "ScriptBehavior.h"

struct WaveSolver : public ScriptBehavior
{
public:
	virtual void Behavior(Script * aOwningScript) override { 
		//std::cout << "Wave Solver!\n"; 
	}
};