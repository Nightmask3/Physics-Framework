#pragma once
#include "Engine.h"
#include "Observer.h"

class EngineStateManager : public Observer
{
	/*----------MEMBER VARIABLES----------*/
public:
	// Engine Reference
	Engine & EngineHandle;
	// Renderer variables
	bool bShouldRenderCollidersAndNormals = false;
	bool bRenderModeWireframe = false;
	bool bShouldRenderMinkowskiDifference = true;
	// Physics variables
	int NumberofIterations;
	bool bShouldRenderSimplex = true;
	bool bUseEulerIntegration = true;
	bool bUseVerletIntegration = false;
	bool bUseRK4Integration = false;
	/*----------MEMBER FUNCTIONS----------*/
public:
	EngineStateManager(Engine & aEngine) : EngineHandle(aEngine) {}
	~EngineStateManager() {}

	Engine const & GetEngine() { return EngineHandle; }
private:
	void Update();

	// Observer functions
	virtual void OnNotify(Event * aEvent) override;

};
