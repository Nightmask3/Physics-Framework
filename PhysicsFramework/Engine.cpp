#include <glm/gtc/random.hpp>

#include "Engine.h"
#include "WindowManager.h"
#include "InputManager.h"
#include "GameObjectFactory.h"
#include "FrameRateController.h"
#include "PhysicsManager.h"
#include "Sprite.h"
#include "Camera.h"

#include <imgui.h>
#include "imgui_impl_glfw_gl3.h"

Engine::Engine()
{
	pResourceManager = std::make_unique<ResourceManager>(*this);
	
	/*-------------- ENGINE INIT EVENT REGISTRATION --------------*/
	Subject EngineInitialized;
	
	// Create the frame rate controller
	pFrameRateController = std::make_unique<FramerateController>(*this);
	// Register it with engine init event list
	EngineInitialized.AddObserver(pFrameRateController.get());

	// Do the same for window manager
	pWindowManager = std::make_unique<WindowManager>(*this);
	// Register window manager
	EngineInitialized.AddObserver(pWindowManager.get());

	// Do the same for input manager...
	pInputManager = std::make_unique<InputManager>(*this);
	EngineInitialized.AddObserver(pInputManager.get());
	// Do the same for the renderer...
	pRenderer = std::make_unique<Renderer>(*this);
	EngineInitialized.AddObserver(pRenderer.get());

	// Physics manager doesn't need init events
	pPhysicsManager = std::make_unique<PhysicsManager>(*this);

	// Do the same for game object factory...
	pGameObjectFactory = std::make_unique<GameObjectFactory>(this);
	EngineInitialized.AddObserver(pGameObjectFactory.get());

	// Adds the engine initialized subject to the main event list (must be done after adding all observers as emplace uses a copy in a map)
	MainEventList.emplace(std::make_pair(EngineEvent::ENGINE_INIT, EngineInitialized));
	
	/*-------------- ENGINE LOAD EVENT REGISTRATION --------------*/
	Subject EngineLoad;
	
	EngineLoad.AddObserver(pRenderer.get());
	EngineLoad.AddObserver(pResourceManager.get());

	MainEventList.emplace(std::make_pair(EngineEvent::ENGINE_LOAD, EngineLoad));

	/*-------------- ENGINE TICK EVENT REGISTRATION --------------*/
	Subject EngineTick;
	// Register input manager to update on the event tick
	EngineTick.AddObserver(pInputManager.get());
	// Do the same for frame rate controller...
	EngineTick.AddObserver(pFrameRateController.get());
	// Do the same for the renderer...
	EngineTick.AddObserver(pRenderer.get());
	// Do the same for the physics manager
	EngineTick.AddObserver(pPhysicsManager.get());

	MainEventList.emplace(std::make_pair(EngineEvent::ENGINE_TICK, EngineTick));

}

int Engine::Init()
{
	EngineEvent InitEvent;
	InitEvent.EventID = EngineEvent::ENGINE_INIT;
	// Notify all listeners to engine init
	MainEventList[EngineEvent::ENGINE_INIT].Notify(this, &InitEvent);
	
	// Create camera and add it to the tick notification list
	Camera * mainCamera = new Camera(*pInputManager, *pFrameRateController);
	MainEventList[EngineEvent::ENGINE_TICK].AddObserver(mainCamera);
	// Set the renderer camera reference
	pRenderer->SetActiveCamera(mainCamera);


	// Setup ImGui binding
	ImGui_ImplGlfwGL3_Init(pWindowManager->GetWindow(), true);
	
	return 0;
}

int Engine::Load()
{
	EngineEvent LoadEvent;
	LoadEvent.EventID = EngineEvent::ENGINE_LOAD;
	// Notify all listeners to engine load
	MainEventList[EngineEvent::ENGINE_LOAD].Notify(this, &LoadEvent);

	GameObject * cube1 = pGameObjectFactory->SpawnGameObjectFromArchetype("Cube.txt");
	GameObject * cube2 = pGameObjectFactory->SpawnGameObjectFromArchetype("Cube.txt");
	cube2->GetComponent<Transform>()->SetPosition(glm::vec3(5.5f, 0.5f, 0.5f));
	
	return 0;
}

int Engine::Tick()
{

	bool show_test_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImColor(114, 144, 154);

	ImGui_ImplGlfwGL3_NewFrame();

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(pWindowManager->GetWindow()))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

		EngineEvent TickEvent;
		TickEvent.EventID = EngineEvent::ENGINE_TICK;
		// Notify all listeners to engine tick 
		MainEventList[EngineEvent::ENGINE_TICK].Notify(this, &TickEvent);
		
		// 1. Show a simple window
		// Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"
		{
			static float f = 0.0f;
			ImGui::Text("Hello, world!");
			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
			ImGui::ColorEdit3("clear color", (float*)&clear_color);
			if (ImGui::Button("Test Window")) show_test_window ^= 1;
			if (ImGui::Button("Another Window")) show_another_window ^= 1;
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		}

		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui::Render();

		/* Swap front and back buffers */
		glfwSwapBuffers(pWindowManager->GetWindow());


		if (pInputManager->isKeyReleased(GLFW_KEY_ESCAPE))
			break;

		/* Poll for and process events */
		glfwPollEvents();

	}

	glfwTerminate();
	return 0;
}


Engine::~Engine()
{
}
