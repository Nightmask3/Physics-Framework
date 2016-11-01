#include "WindowManager.h"
WindowManager::WindowManager()
{
}


WindowManager::~WindowManager()
{
}

int WindowManager::InitializeWindow()
{
	/* Initialize the library */
	if (!glfwInit())
		return -1;
	
	/* Create a windowed mode window and its OpenGL context */
	pWindow = glfwCreateWindow(640, 480, "Physics Framework", NULL, NULL);
	if (!pWindow)
	{
		std::cout << "Could not Initialize Window!";
		glfwTerminate();
		return -1;
	}
	/* Make the window's context current */
	glfwMakeContextCurrent(pWindow);
}

void WindowManager::OnNotify(Object * object, Event * event)
{
	// Check if this is an Engine event
	EngineEvent * engineEvent = static_cast<EngineEvent *>(event);
	if (engineEvent)
	{
		switch (engineEvent->EventID)
		{
			case EngineEvent::EventList::ENGINE_INIT:
			{
				InitializeWindow();
			}
		}
	}
}
