#include "FrameRateController.h"

FrameRateController::FrameRateController()
{
}

void FrameRateController::InitializeFrameRateController()
{
	TotalTime = NewTime = CurrentTime = DeltaTime = 0.0;
	FixedDelta = 0.016; 
}

void FrameRateController::SetFrameRateLimit(unsigned int Limit)
{
	DeltaTime = 1.0f / Limit;
	// Starts the clock
	glfwSetTime(0);
	CurrentTime = glfwGetTime();
}
void FrameRateController::UpdateFrameTime()
{
	NewTime = glfwGetTime();
	DeltaTime = NewTime - CurrentTime;
	CurrentTime = NewTime;
	TotalTime += DeltaTime;
}

FrameRateController::~FrameRateController()
{
}

void FrameRateController::OnNotify(Object * object, Event * event)
{
	// Check if this is an Engine event
	EngineEvent * engineEvent = static_cast<EngineEvent *>(event);
	if (engineEvent)
	{
		switch (engineEvent->EventID)
		{
			case EngineEvent::EventList::ENGINE_INIT:
			{
				InitializeFrameRateController();
				break;
			}
			case EngineEvent::EventList::ENGINE_TICK:
			{
				UpdateFrameTime();
				break;
			}
		}
	}
}
