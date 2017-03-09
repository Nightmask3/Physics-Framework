#pragma once
#include <iostream>

#include "Engine.h"
#include "Observer.h"

class WindowManager : public Observer
{
	/*----------MEMBER VARIABLES----------*/
private:
	static GLFWwindow * pWindow;

	/*------------------------------- ENGINE REFERENCE -------------------------------*/
	Engine const & EngineHandle;
public:
	static int Width;
	static int Height;
	/*----------MEMBER FUNCTIONS----------*/
public:
	WindowManager(Engine const & aEngine) : EngineHandle(aEngine) {}
	~WindowManager();

	inline GLFWwindow * GetWindow() const { return pWindow; }
	Engine const & GetEngine() { return EngineHandle; }

	// Window manager functions
	int InitializeWindow();
	// Observer functions
	virtual void OnNotify(Object * object, Event * event) override;

	// Window resize 
	static void WindowResizeCallback(GLFWwindow * aWindow, int aWidth, int aHeight);

};

