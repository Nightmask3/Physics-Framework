#pragma once
#include <iostream>

#include "Engine.h"
#include "Observer.h"

class WindowManager : public Observer
{
	/*----------MEMBER VARIABLES----------*/
private:
	GLFWwindow * pWindow;

	/*------------------------------- ENGINE REFERENCE -------------------------------*/
	Engine const & EngineHandle;
public:
	int Width = 1024;
	int Height = 768;
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

