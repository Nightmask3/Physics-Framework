#pragma once

#include <map>
#include <memory>
// GLEW
#include <GL/glew.h>
// GLFW
#include <GLFW/glfw3.h>
// Entity classes
#include "Object.h"
// Event classes
#include "Event.h"
#include "Subject.h"

// Forward Declarations
class WindowManager;
class InputManager;
class FramerateController;
class GameObjectFactory;
class ResourceManager;
class Renderer;
class PhysicsManager;
class ImGuiManager;

// The main engine events that have Subjects created from the start
class EngineEvent : public Event
{
public:
	enum EventList
	{
		ENGINE_INIT,
		ENGINE_LOAD,
		ENGINE_TICK,
		ENGINE_UNLOAD,
		ENGINE_EXIT
	};
	EventList EventID;
	EngineEvent() {};
	~EngineEvent() {};
};

class Engine : public Object
{
	/*----------MEMBER VARIABLES----------*/
private:
	std::unique_ptr<WindowManager> pWindowManager;
	std::unique_ptr<InputManager> pInputManager;
	std::unique_ptr<FramerateController> pFrameRateController;
	std::unique_ptr<GameObjectFactory> pGameObjectFactory;
	std::unique_ptr<ResourceManager> pResourceManager;
	std::unique_ptr<Renderer> pRenderer;
	std::unique_ptr<PhysicsManager> pPhysicsManager;
	std::unique_ptr<ImGuiManager> pImGuiManager;
	// A map of the main engine events
	std::map<EngineEvent::EventList, Subject> MainEventList;

	/*----------MEMBER FUNCTIONS----------*/
public:
	Engine();
	void Init();
	void Load();
	int Unload();
	void Exit();
	void Tick();
	virtual ~Engine();
	std::map<EngineEvent::EventList, Subject> & GetMainEventList() { return MainEventList; }

	inline WindowManager & GetWindowManager()  const { return *pWindowManager; }
	inline InputManager & GetInputManager() const { return *pInputManager; }
	inline FramerateController & GetFramerateController() const { return *pFrameRateController; }
	inline GameObjectFactory & GetGameObjectFactory() const { return *pGameObjectFactory; }
	inline ResourceManager & GetResourceManager() const { return *pResourceManager; }
	inline Renderer & GetRenderer() const { return *pRenderer; }
	inline PhysicsManager & GetPhysicsManager() const { return *pPhysicsManager; }
	inline ImGuiManager & GetImGuiManager() const { return *pImGuiManager; }

	inline WindowManager & GetWindowManager() { return *pWindowManager; }
	inline InputManager & GetInputManager() { return *pInputManager; }
	inline FramerateController & GetFramerateController() { return *pFrameRateController; }
	inline GameObjectFactory & GetGameObjectFactory() { return *pGameObjectFactory; }
	inline ResourceManager & GetResourceManager() { return *pResourceManager; }
	inline Renderer & GetRenderer() { return *pRenderer; }
	inline PhysicsManager & GetPhysicsManager() { return *pPhysicsManager; }
	inline ImGuiManager & GetImGuiManager() { return *pImGuiManager; }

};

