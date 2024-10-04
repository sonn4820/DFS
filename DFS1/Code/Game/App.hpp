#pragma once
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Window/Window.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/DebugRender.hpp"
class Game;

class App {

public:
	App();
	~App();

	void Startup();
	void Shutdown();
	void Run();
	void RunFrame();

	bool IsQuitting() const { return m_isQuitting; }
	bool HandleQuitRequested();

public:
	void BeginFrame();
	void Update(float deltaSeconds);
	void Render() const;
	void EndFrame();
	static bool Event_Quit(EventArgs& args);

	bool m_isQuitting = false;
	
private:
	void ConsoleTutorial();
	void InitializeGameConfig(const char* filePath);
};