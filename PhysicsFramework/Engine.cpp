#include <glm/gtc/random.hpp>

#include "Engine.h"
#include "WindowManager.h"
#include "InputManager.h"
#include "GameObjectFactory.h"
#include "FrameRateController.h"
#include "PhysicsManager.h"
#include "Sprite.h"
#include "Camera.h"

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
	cube2->GetComponent<Transform>()->SetPosition(glm::vec3(0.f, 5.f, 0.f));
	return 0;
}

int Engine::Tick()
{

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(pWindowManager->GetWindow()))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

		EngineEvent TickEvent;
		TickEvent.EventID = EngineEvent::ENGINE_TICK;
		// Notify all listeners to engine tick 
		MainEventList[EngineEvent::ENGINE_TICK].Notify(this, &TickEvent);
		
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
