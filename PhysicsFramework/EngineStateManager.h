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
	bool bShouldRenderCollidersAndNormals;
	bool bRenderModeWireframe;
	bool bShouldRenderMinkowskiDifference;
	// Physics variables
	int NumberofIterations;
	bool bShouldRenderSimplex;
	bool bUseEulerIntegration;
	bool bUseVerletIntegration;
	bool bUseRK4Integration;
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
