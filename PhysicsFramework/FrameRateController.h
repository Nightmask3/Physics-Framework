#pragma once
#include "Engine.h"
class FramerateController : public Observer
{
	/*----------MEMBER FUNCTIONS----------*/
public:
	FramerateController(Engine const & aEngine) : EngineHandle(aEngine) {}
	~FramerateController();

	virtual void OnNotify(Object * object, Event * event);

	// Getters
	inline float GetFPSLimit() { return 1 / FixedDelta; }
	inline float GetFPS() { return 1 / DeltaTime; }
	inline float GetDeltaTime() { return DeltaTime; }
	inline float GetTotalTime() { return TotalTime; }

	Engine const & GetEngine() { return EngineHandle; }

private:
	void InitializeFrameRateController();
	void SetFrameRateLimit(unsigned int Limit);
	void UpdateFrameTime();
	/*----------MEMBER VARIABLES----------*/
private:
	// Total time accumulator
	float TotalTime;
	// Frame time limit
	float FixedDelta;
	// Actual time since last frame
	float DeltaTime;
	// Current time in seconds
	float CurrentTime;
	// New time in seconds
	float NewTime;
	/*------------------------------- ENGINE REFERENCE -------------------------------*/
	Engine const & EngineHandle;

};

