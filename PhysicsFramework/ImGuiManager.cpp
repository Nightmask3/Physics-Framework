#include "ImGuiManager.h"
#include "WindowManager.h"

#include "WindowMenuBarWidget.h"

// Included here to actually have the implementation for the linker
#include "imgui.cpp"
#include "imgui_draw.cpp"
#include "imgui_demo.cpp"
#include "imgui_impl_glfw_gl3.cpp"

ImGuiManager::~ImGuiManager()
{
	// Cleanup
	ImGui_ImplGlfwGL3_Shutdown();
}

void ImGuiManager::ImGuiNewFrame()
{
	ImGui_ImplGlfwGL3_NewFrame();
}

void ImGuiManager::ImGuiRender()
{
	ImGui::Render();
}

void ImGuiManager::DrawWidgets()
{
	bool show_test_window = true;
	// 1. Show a simple window
	// Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"
	{
		static float f = 0.0f;
		ImGui::Text("Use WASD & QE to move the camera. \nTAB activates debug draw");
		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
		if (ImGui::Button("Test Window")) show_test_window ^= 1;
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}



	// 2. Show the ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
	/*if (show_test_window)
	{
		ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
		ImGui::ShowTestWindow(&show_test_window);
	}*/

}

void ImGuiManager::OnNotify(Object * object, Event * event)
{
	EngineEvent * engineEvent = nullptr;
	engineEvent = dynamic_cast<EngineEvent *>(event);

	if (engineEvent)
	{
		if (engineEvent->EventID == EngineEvent::EventList::ENGINE_INIT)
		{
			// Setup ImGui binding
			ImGui_ImplGlfwGL3_Init(EngineHandle.GetWindowManager().GetWindow(), true);
		}
		else if (engineEvent->EventID == EngineEvent::EventList::ENGINE_TICK)
		{
			DrawWidgets();
		}
		else if (engineEvent->EventID == EngineEvent::EventList::ENGINE_EXIT)
		{
			// Cleanup
			ImGui_ImplGlfwGL3_Shutdown();
		}
		return;
	}
}
