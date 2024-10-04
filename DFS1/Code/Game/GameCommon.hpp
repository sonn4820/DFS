#pragma once
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/RaycastUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/JobSystem.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/UI/UISystem.hpp"
#include "Engine/UI/Canvas.hpp"
#include "Engine/UI/Button.hpp"
#include "Engine/UI/Panel.hpp"
#include "Engine/UI/Slider.hpp"
#include "Engine/UI/Text.hpp"
#include "Engine/UI/Checkbox.hpp"
#include "Game/App.hpp"
#include "Game/Game.hpp"
#include <math.h>

extern App* g_theApp;
extern Renderer* g_theRenderer;
extern InputSystem* g_theInput;
extern AudioSystem* g_theAudio;
extern RandomNumberGenerator* g_theRNG;
extern BitmapFont* g_theFont;
extern UISystem* g_theUI;
extern Game* g_theGame;
extern JobSystem* g_theJobSystem;
extern bool g_debugDrawing;
extern bool g_gameplayMode;

constexpr float WORLD_SIZE_X = 200.f;
constexpr float WORLD_SIZE_Y = 100.f;
constexpr float MAX_SHAKE = 10.f;
constexpr float SHAKE_REDUCTION_PER_SEC = 7.f;

constexpr float BALL_RADIUS = 0.5f;
constexpr float BALL_MASS = 58.f;

constexpr float FORCE_RATE = 40.f;
constexpr float SPIN_RATE = 500.f;
constexpr float DIR_AND_POS_RATE = 0.01f;
constexpr float SPIN_MAX_FORCE = 35000.f;
constexpr float THROW_MAX_FORCE = 4000.f;
constexpr float MODIFIED_GRAVITY_RATE = -9.8f * 2.5f;

constexpr float GROUND_STATIC_FRICTION = 0.2f;

constexpr float RAY_CAST_RADIUS = 5.f;
constexpr float RAY_CAST_LENGTH = 2.f;
