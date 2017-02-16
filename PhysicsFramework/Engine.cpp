#include <glm/gtc/random.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include "Engine.h"
#include "WindowManager.h"
#include "InputManager.h"
#include "GameObjectFactory.h"
#include "FrameRateController.h"
#include "PhysicsManager.h"
#include "Sprite.h"
#include "Camera.h"
#include "ImGuiManager.h"

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
	pGameObjectFactory = std::make_unique<GameObjectFactory>(this); // Requires pointer and not const ref
	EngineInitialized.AddObserver(pGameObjectFactory.get());

	// Do the same for ImGui manager
	pImGuiManager = std::make_unique<ImGuiManager>(*this);
	EngineInitialized.AddObserver(pImGuiManager.get());

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
	// Do the same for the physics manager...
	EngineTick.AddObserver(pPhysicsManager.get());
	// Do the same for the ImGui Manager...
	EngineTick.AddObserver(pImGuiManager.get());

	MainEventList.emplace(std::make_pair(EngineEvent::ENGINE_TICK, EngineTick));
	
	/*-------------- ENGINE EXIT EVENT REGISTRATION --------------*/
	Subject EngineExit;
	// Register window manager to get an exit event
	EngineExit.AddObserver(pWindowManager.get());
	// Do the same for ImGuiManager...
	EngineExit.AddObserver(pImGuiManager.get());

	MainEventList.emplace(std::make_pair(EngineEvent::ENGINE_EXIT, EngineExit));

}

void Engine::Init()
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

	return;
}

void Engine::Load()
{
	EngineEvent LoadEvent;
	LoadEvent.EventID = EngineEvent::ENGINE_LOAD;
	// Notify all listeners to engine load
	MainEventList[EngineEvent::ENGINE_LOAD].Notify(this, &LoadEvent);

	GameObject * pivot = pGameObjectFactory->SpawnGameObject();
	Mesh * pivotMesh = pResourceManager->ImportMesh(std::string("Pivot.fbx"));
	pivot->GetComponent<Transform>()->SetScale(glm::vec3(-0.1f));
	pivot->AddComponent(pivotMesh);
	pivotMesh->SetOwner(pivot);

	GameObject * cube3 = pGameObjectFactory->SpawnGameObject();
	cube3->GetComponent<Transform>()->SetPosition(glm::vec3(0.0f, 5.0f, 0.0f));
	Mesh * cube3Mesh = pResourceManager->ImportMesh(std::string("Cube.fbx"));
	cube3->AddComponent(cube3Mesh);
	cube3Mesh->SetOwner(cube3);

	GameObject * cube4 = pGameObjectFactory->SpawnGameObject();
	cube4->GetComponent<Transform>()->SetPosition(glm::vec3(0.0f, -5.0f, 0.0f));
	Mesh * cube4Mesh = pResourceManager->ImportMesh(std::string("Cube.fbx"));
	cube4->AddComponent(cube4Mesh);
	cube4Mesh->SetOwner(cube4);

	GameObject * minkowskiDifference = pGameObjectFactory->SpawnGameObject();
	std::vector<Vertex> MinkowskiDifferenceVertices;
	Mesh * minkowskiMesh = pGameObjectFactory->SpawnComponent<Mesh>();
	minkowskiMesh->SetOwner(minkowskiDifference);
	minkowskiDifference->AddComponent(minkowskiMesh);
	int size = cube3->GetComponent<Mesh>()->Vertices.size() * cube4->GetComponent<Mesh>()->Vertices.size();
	MinkowskiDifferenceVertices.reserve(size);

	// Update camera values before constructing view matrix
	glm::vec3 cameraPosition = pRenderer->pActiveCamera->GetCameraPosition();
	glm::vec3 cameraTarget = pRenderer->pActiveCamera->GetCameraLookDirection();
	glm::vec3 upVector = pRenderer->pActiveCamera->GetCameraUpDirection();

	/*-------------------------------- VIEW MATRIX -------------------------------*/
	glm::mat4 view;
	view = glm::lookAt(
		cameraPosition,
		cameraTarget,
		upVector
	);

	glm::mat4 projection;
	projection = glm::perspective(
		45.f, // The horizontal Field of View, in degrees : the amount of "zoom". Think "camera lens". Usually between 90° (extra wide) and 30° (quite zoomed in)
		4.0f / 3.0f, // Aspect Ratio. Depends on the size of your window. Notice that 4/3 == 800/600 == 1280/960, sounds familiar ?
		0.1f,        // Near clipping plane. Keep as big as possible, or you'll get precision issues.
		100.0f       // Far clipping plane. Keep as little as possible.
	);

	for (int i = 0; i < cube3Mesh->Vertices.size(); ++i)
	{
		// Calculate the MVP matrix and set the matrix uniform
		glm::mat4 mvp;
		glm::mat4 model;
		glm::mat4 translate, scale;
		translate = glm::translate(cube3->GetComponent<Transform>()->GetPosition());
		scale = glm::scale(cube3->GetComponent<Transform>()->GetScale() * 1.25f);
		model = translate * scale;
		mvp = projection * view * model;

		Vertex newVertex;
		newVertex.Position = glm::vec3( mvp * glm::vec4(cube3Mesh->Vertices[i].Position, 1) - glm::vec4(cube4Mesh->Vertices[i].Position, 1));
		MinkowskiDifferenceVertices.push_back(newVertex);
	}
	for (int i = 0; i < cube4Mesh->Vertices.size(); ++i)
	{
		// Calculate the MVP matrix and set the matrix uniform
		glm::mat4 mvp;
		glm::mat4 model;
		glm::mat4 translate, scale;
		translate = glm::translate(cube3->GetComponent<Transform>()->GetPosition());
		scale = glm::scale(cube3->GetComponent<Transform>()->GetScale() * 1.25f);
		model = translate * scale;
		mvp = projection * view * model;

		Vertex newVertex;
		newVertex.Position = glm::vec3(mvp * glm::vec4(cube4Mesh->Vertices[i].Position, 1) - glm::vec4(cube3Mesh->Vertices[i].Position, 1));
		MinkowskiDifferenceVertices.push_back(newVertex);
	}

	minkowskiMesh->SetVertices(MinkowskiDifferenceVertices);

	return;
}

void Engine::Exit()
{
	EngineEvent ExitEvent;
	ExitEvent.EventID = EngineEvent::ENGINE_EXIT;
	// Notify all listeners to engine exit
	MainEventList[EngineEvent::ENGINE_EXIT].Notify(this, &ExitEvent);

	return;
}

void Engine::Tick()
{

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(pWindowManager->GetWindow()))
	{
		// Needs to be called at start of every frame
		ImGuiManager::ImGuiNewFrame();

		glClear(GL_COLOR_BUFFER_BIT);

		EngineEvent TickEvent;
		TickEvent.EventID = EngineEvent::ENGINE_TICK;
		// Notify all listeners to engine tick 
		MainEventList[EngineEvent::ENGINE_TICK].Notify(this, &TickEvent);
		
		// Draws GUI widgets on top of everything else
		ImGuiManager::ImGuiRender();

		/* Swap front and back buffers */
		glfwSwapBuffers(pWindowManager->GetWindow());


		if (pInputManager->isKeyReleased(GLFW_KEY_ESCAPE))
			break;

		/* Poll for and process events */
		glfwPollEvents();

	}

	Exit();
	return;
}


Engine::~Engine()
{
}
