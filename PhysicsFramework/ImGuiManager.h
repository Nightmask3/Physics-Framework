#pragma once
#include <iostream>

#include "Engine.h"
#include "Observer.h"

// ImGui headers, for access to the ImGui namespace to anything that includes ImGuiManager
#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"

class WindowMenuBarWidget;

class ImGuiManager : public Observer
{
	/*----------MEMBER VARIABLES----------*/
private:
	
	/*------------------------------- ENGINE REFERENCE -------------------------------*/
	Engine const & EngineHandle;

	/*----------MEMBER FUNCTIONS----------*/
private:
	void DrawWidgets();
	
public:
	ImGuiManager(Engine const & aEngine) : EngineHandle(aEngine) {}
	~ImGuiManager();

	Engine const & GetEngine() { return EngineHandle; }

	static void ImGuiNewFrame();
	static void ImGuiRender();

	// Observer functions
	virtual void OnNotify(Object * object, Event * event) override;
};
