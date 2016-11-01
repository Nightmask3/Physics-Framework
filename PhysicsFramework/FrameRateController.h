#pragma once
#include "Engine.h"
class FrameRateController : public Observer
{
	/*----------MEMBER FUNCTIONS----------*/
public:
	FrameRateController();
	~FrameRateController();
	virtual void OnNotify(Object * object, Event * event);
	// Getters
	inline float GetFPSLimit() { return 1 / FixedDelta; }
	inline float GetFPS() { return 1 / DeltaTime; }
	inline float GetDeltaTime() { return DeltaTime; }
	inline float GetTotalTime() { return TotalTime; }

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
};

