#pragma once
#include "glm/vec2.hpp"

#include "Engine.h"
#include "Observer.h"
class InputManager : public Observer
{
	/*----------MEMBER FUNCTIONS----------*/
public:
	InputManager(WindowManager const & windowManager);
	virtual ~InputManager();

	// Observer functions
	virtual void OnNotify(Object * object, Event * event) override;

	// MOUSE HANDLING STUFF
	bool isMouseButtonPressed(int key);
	
	void Tick();
	
	// Key State Checking
	bool isKeyPressed(int key) const;
	bool isKeyReleased(int key) const;
	bool isKeyTriggered(int key) const;

	void InitializeKeyboardState();
	/*----------MEMBER VARIABLES----------*/
private:
	bool keyboardStatePrev[GLFW_KEY_LAST];	// Holds the state of all the keyboard keys in the previous frame
	bool keyboardStateCurr[GLFW_KEY_LAST];	// Holds the state of all the keyboard keys in the current frame

	WindowManager const & pWindowManager;
public:
	glm::vec2 GetMousePosition();

	// ARRAY OF CONTROLLER COMPONENTS
	//std::vector<Component *> ControllerList;

};

