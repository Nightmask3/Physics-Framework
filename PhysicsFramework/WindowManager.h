#pragma once
#include <iostream>

#include "Engine.h"
#include "Observer.h"

class WindowManager : public Observer
{
	/*----------MEMBER FUNCTIONS----------*/
public:
	inline GLFWwindow * GetWindow() const { return pWindow; }
	WindowManager();
	~WindowManager();
	// Window manager functions
	int InitializeWindow();
	
	// Observer functions
	virtual void OnNotify(Object * object, Event * event) override;

	/*----------MEMBER VARIABLES----------*/
private:
	GLFWwindow * pWindow;
};

