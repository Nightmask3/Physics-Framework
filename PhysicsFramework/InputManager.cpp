#include "InputManager.h"
#include "WindowManager.h"
InputManager::InputManager(WindowManager const & windowManager) : pWindowManager(windowManager)
{}

void InputManager::InitializeKeyboardState()
{
	for (int i = 0; i < GLFW_KEY_LAST; ++i) // Initializes the value of all keyboard keys to false currently and previously
	{
		keyboardStatePrev[i] = false;
		keyboardStateCurr[i] = false;
	}
}


InputManager::~InputManager()
{
}

void InputManager::OnNotify(Object * object, Event * event)
{
	// Check if this is an Engine event
	EngineEvent * engineEvent = static_cast<EngineEvent *>(event);
	if (engineEvent)
	{
		switch (engineEvent->EventID)
		{
			case EngineEvent::EventList::ENGINE_INIT:
			{
				InitializeKeyboardState();
				break;
			}
			case EngineEvent::EventList::ENGINE_TICK:
			{
				Tick();
				break;
			}
		}
	}
}

// Used to check the input state of each 'named' keyboard key
void InputManager::Tick()
{						
	for (int key = 0; key < GLFW_KEY_LAST; ++key)
	{
		int state = glfwGetKey(pWindowManager.GetWindow(), key);

		// If pressed and currently false, set to currently true
		if (state == GLFW_PRESS && keyboardStateCurr[key] == false)
		{
			keyboardStateCurr[key] = true;
		}
		// If pressed and currently true, set to previously true
		else if (state == GLFW_PRESS && keyboardStateCurr[key] == true)
		{
			keyboardStatePrev[key] = true;
		}
		// if not pressed and currently true, set to currently false
		else if (state == GLFW_RELEASE && keyboardStateCurr[key] == true)
		{
			keyboardStateCurr[key] = false;
			keyboardStatePrev[key] = true;
		}
		// if not pressed and previously true, set to previously false
		else if (state == GLFW_RELEASE && keyboardStatePrev[key] == true)
		{
			keyboardStateCurr[key] = false;
			keyboardStatePrev[key] = false;
		}
	}
}

bool InputManager::isKeyPressed(int key) const
{
	return keyboardStateCurr[key] && keyboardStatePrev[key];	// When a key is pressed in both this frame and previous frame returns true
}

bool InputManager::isKeyReleased(int key) const
{
	return !keyboardStateCurr[key] && keyboardStatePrev[key]; 	// When key is not pressed in this frame but pressed in previous frame return true
}

bool InputManager::isKeyTriggered(int key) const
{
	return keyboardStateCurr[key] && !keyboardStatePrev[key];	// True in current frame but not in previous frame
}

