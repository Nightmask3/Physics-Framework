#pragma once
#include <iostream>

#include "Engine.h"
#include "Observer.h"

class WindowManager : public Observer
{
	/*----------MEMBER VARIABLES----------*/
private:
	GLFWwindow * pWindow;
	int WindowSizeX = 1024;
	int WindowSizeY = 768;
	/*------------------------------- ENGINE REFERENCE -------------------------------*/
	Engine const & EngineHandle;

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
};

