#include "Game/Game.hpp"
#include "Game/BasketballCourt.hpp"
#include "Game/Player.hpp"
#include "Game/GameCommon.hpp"
#include "ThirdParty/SquirrelNoise/SmoothNoise.hpp"

bool g_debugDrawing = false;

constexpr float MAX_VOLUME = 1.f;

Game::Game()
{
	m_screenCamera.SetOrthographicView(Vec2(0, 0), Vec2(g_gameConfigBlackboard.GetValue("screenSizeX", 1600.f), g_gameConfigBlackboard.GetValue("screenSizeY", 800.f)));
	m_clock = new Clock(*Clock::s_theSystemClock);
}
//..............................
Game::~Game()
{
	delete m_menuCanvas;
	m_menuCanvas = nullptr;

	delete m_resultCanvas;
	m_resultCanvas = nullptr;
}
//..............................
void Game::Startup()
{
	LoadAttractData();
	LoadGamePlayData();
	InitMainMenuUI();
	InitTimerResultUI();
	SwitchState(GameState::ATTRACT_MODE);

}
//..............................
void Game::Shutdown()
{
	delete[] m_gameMusics;
	m_gameMusics = nullptr;

	if (m_map)
	{
		m_map->Shutdown();
	}
}

void Game::LoadAttractData()
{
	m_menuTexture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/menu.jpg");
	m_menuMusic = g_theAudio->CreateOrGetSound(g_gameConfigBlackboard.GetValue("menuMusic", "Data/Audio/TestSound.mp3"));
}

void Game::LoadGamePlayData()
{
	m_ballTexture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/ball.png");
	m_courtTexture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/court.png");
	m_skyboxTexture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/skybox.png");
	m_netTexture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/net.png");
	m_hoopCylTexture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/hoopcyl.jpg");
	m_hoopBoardTexture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/hoopboard.jpg");
	m_hoopBasketTexture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/red.png");

	m_crosshairTexture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/crosshair.png");

	m_lebronBlockerTexture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/lebron.png");
	m_lukaBlockerTexture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/luka.png");
	m_damianBlockerTexture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/damian.png");

	m_gameMusics = new SoundID[5];
	m_gameMusics[0] = g_theAudio->CreateOrGetSound(g_gameConfigBlackboard.GetValue("gameMusic1", "Data/Audio/TestSound.mp3"));
	m_gameMusics[1] = g_theAudio->CreateOrGetSound(g_gameConfigBlackboard.GetValue("gameMusic2", "Data/Audio/TestSound.mp3"));
	m_gameMusics[2] = g_theAudio->CreateOrGetSound(g_gameConfigBlackboard.GetValue("gameMusic3", "Data/Audio/TestSound.mp3"));
	m_gameMusics[3] = g_theAudio->CreateOrGetSound(g_gameConfigBlackboard.GetValue("gameMusic4", "Data/Audio/TestSound.mp3"));
	m_gameMusics[4] = g_theAudio->CreateOrGetSound(g_gameConfigBlackboard.GetValue("gameMusic5", "Data/Audio/TestSound.mp3"));

	m_buttonClickSound = g_theAudio->CreateOrGetSound(g_gameConfigBlackboard.GetValue("buttonClick", "Data/Audio/TestSound.mp3"));
	m_sliderSound = g_theAudio->CreateOrGetSound(g_gameConfigBlackboard.GetValue("slider", "Data/Audio/TestSound.mp3"));
	m_ballBounceGroundSound = g_theAudio->CreateOrGetSound(g_gameConfigBlackboard.GetValue("bounceGround", "Data/Audio/TestSound.mp3"));
	m_ballBounceBackboardSound = g_theAudio->CreateOrGetSound(g_gameConfigBlackboard.GetValue("bouncBackboard", "Data/Audio/TestSound.mp3"));
	m_ballBounceNetSound = g_theAudio->CreateOrGetSound(g_gameConfigBlackboard.GetValue("bounceNet", "Data/Audio/TestSound.mp3"));
	m_scoreSound = g_theAudio->CreateOrGetSound(g_gameConfigBlackboard.GetValue("scoreSound", "Data/Audio/TestSound.mp3"));

	m_staticBlockerSound = g_theAudio->CreateOrGetSound(g_gameConfigBlackboard.GetValue("staticBlocker", "Data/Audio/TestSound.mp3"));
	m_continuousBlockerSound = g_theAudio->CreateOrGetSound(g_gameConfigBlackboard.GetValue("continuousBlocker", "Data/Audio/TestSound.mp3"));
	m_timerBlockerSound = g_theAudio->CreateOrGetSound(g_gameConfigBlackboard.GetValue("timerBlocker", "Data/Audio/TestSound.mp3"));
}

//----------------------------------------------------------------------------------------------------------------------------------------
// UPDATE

void Game::Update(float deltaSeconds)
{
	m_secondIntoMode += deltaSeconds;

	if (GetCurrentState() == GameState::PLAY_MODE)
	{
		UpdatePlayMode(deltaSeconds);
	}
	if (GetCurrentState() == GameState::ATTRACT_MODE)
	{
		UpdateAttractMode(deltaSeconds);
	}
	if (GetCurrentState() == GameState::RESULT_MODE)
	{
		UpdateResultMode(deltaSeconds);
	}

	UpdateCamera();
}

void Game::UpdatePhysics(float fixedDeltaSeconds)
{
	if (m_map)
	{
		m_map->UpdatePhysics(fixedDeltaSeconds);
	}
}

//..............................
void Game::UpdatePlayMode(float deltaSeconds)
{
	if (m_currentGameMode == TIMER)
	{
		m_countdownReadyTimer -= deltaSeconds;
		if (m_countdownReadyTimer < 0.f)
		{
			m_countdownTimeChallenge -= deltaSeconds;
			if (m_countdownTimeChallenge < 0)
			{
				SwitchState(GameState::RESULT_MODE);
			}
		}
	}
	m_timeDebt += deltaSeconds;
	while (m_timeDebt >= m_fixedTimeStep)
	{
		UpdatePhysics(m_fixedTimeStep);
		m_timeDebt -= m_fixedTimeStep;
	}

	if (g_theInput->WasKeyJustPressed(KEYCODE_F1))
	{
		g_debugDrawing = !g_debugDrawing;
	}
	if (g_theInput->WasKeyJustPressed(KEYCODE_F2))
	{
		g_gameplayMode = !g_gameplayMode;
		g_debugDrawing = true;
	}
	if (g_theInput->WasKeyJustPressed('L'))
	{
		m_currentGameMusicIndex++;
		if (m_currentGameMusicIndex > 4) m_currentGameMusicIndex = 0;
		PlayMusic(m_gameMusics[m_currentGameMusicIndex], false);
	}
	if (g_theInput->WasKeyJustPressed('K'))
	{
		m_currentGameMusicIndex--;
		if (m_currentGameMusicIndex < 0) m_currentGameMusicIndex = 4;
		PlayMusic(m_gameMusics[m_currentGameMusicIndex], false);
	}
	if (g_theInput->WasKeyJustPressed(KEYCODE_ESCAPE) || g_theInput->GetController(0).WasButtonJustPressed(XBOX_BUTTON_BACK))
	{
		SwitchState(GameState::ATTRACT_MODE);
	}

	if (m_map)
	{
		m_map->Update(deltaSeconds);
	}

	GameMusicDJ();
}
//..............................
void Game::UpdateAttractMode(float deltaSeconds)
{
	m_menuCanvas->Update(deltaSeconds);
}

void Game::UpdateResultMode(float deltaSeconds)
{
	m_resultCanvas->Update(deltaSeconds);
}

//..............................
void Game::UpdateCamera()
{
	if (g_gameplayMode && m_map)
	{
		float seed = g_theRNG->RollRandomFloatInRange(-m_screenShakeAmount, m_screenShakeAmount);
		float shakeX = Compute1dPerlinNoise(seed, 2.f, 9);
		float shakeY = Compute1dPerlinNoise(seed + 1, 2.f, 9);
		float shakeZ = Compute1dPerlinNoise(seed + 2, 2.f, 9);
		m_map->m_player->GetCamera()->Rotate3D(EulerAngles(shakeX, shakeY, shakeZ));
		m_screenShakeAmount -= SHAKE_REDUCTION_PER_SEC * Clock::s_theSystemClock->GetDeltaSeconds();
		m_screenShakeAmount = Clamp(m_screenShakeAmount, 0.f, MAX_SHAKE);
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------
// RENDER 

void Game::Render() const
{
	if (GetCurrentState() == GameState::PLAY_MODE)
	{
		RenderPlayMode();
		DebugRenderWorld(*m_map->m_player->GetCamera());
	}
	if (GetCurrentState() == GameState::ATTRACT_MODE)
	{
		RenderAttractMode();
	}
	if (GetCurrentState() == GameState::RESULT_MODE)
	{
		RenderResultMode();
	}
	RenderScreenWorld();

	DebugRenderScreen(m_screenCamera);
}
//..............................

void Game::RenderPlayMode() const
{
	g_theRenderer->ClearScreen(Rgba8(0, 0, 0, 255));

	m_map->Render();


	g_theRenderer->BeginCamera(m_screenCamera);
	m_map->m_player->RenderUI();
	g_theRenderer->EndCamera(m_screenCamera);
}
//..............................
void Game::RenderAttractMode() const
{
	g_theRenderer->ClearScreen(Rgba8(0, 0, 0, 255));
	g_theRenderer->BeginCamera(m_screenCamera);

	g_theRenderer->SetDepthStencilMode(DepthMode::DISABLED);
	g_theRenderer->SetBlendMode(BlendMode::ALPHA);
	g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_NONE);
	g_theRenderer->SetSamplerMode(SampleMode::POINT_CLAMP);
	g_theRenderer->SetModelConstants();

	std::vector<Vertex_PCU> imageVerts;
	AddVertsForAABB2D(imageVerts, AABB2(400.f, 0.f, 1600.f, 800.f));
	g_theRenderer->BindTexture(m_menuTexture);
	g_theRenderer->DrawVertexArray(imageVerts.size(), imageVerts.data());

	std::vector<Vertex_PCU> textVerts;
	g_theFont->AddVertsForTextInBox2D(textVerts, AABB2(100, 600, 1000, 700), 70.f, "D", Rgba8::COLOR_RED, 1.f, Vec2(0, 0.5f));
	g_theFont->AddVertsForTextInBox2D(textVerts, AABB2(175, 600, 1000, 700), 40.f, "ribble,", Rgba8::COLOR_WHITE, 1.f, Vec2(0, 0.38f));
	g_theFont->AddVertsForTextInBox2D(textVerts, AABB2(100, 500, 1000, 600), 70.f, "F", Rgba8::COLOR_RED, 1.f, Vec2(0, 0.5f));
	g_theFont->AddVertsForTextInBox2D(textVerts, AABB2(175, 500, 1000, 600), 40.f, "eint,", Rgba8::COLOR_WHITE, 1.f, Vec2(0, 0.38f));
	g_theFont->AddVertsForTextInBox2D(textVerts, AABB2(100, 400, 1000, 500), 70.f, "S", Rgba8::COLOR_RED, 1.f, Vec2(0, 0.5f));
	g_theFont->AddVertsForTextInBox2D(textVerts, AABB2(175, 400, 1000, 500), 40.f, "hoot!", Rgba8::COLOR_WHITE, 1.f, Vec2(0, 0.38f));


	g_theRenderer->BindTexture(&g_theFont->GetTexture());
	g_theRenderer->DrawVertexArray(textVerts.size(), textVerts.data());

	m_menuCanvas->Render();

	g_theRenderer->EndCamera(m_screenCamera);
}

void Game::RenderResultMode() const
{
	g_theRenderer->ClearScreen(Rgba8(0, 0, 0, 255));
	g_theRenderer->BeginCamera(m_screenCamera);

	std::vector<Vertex_PCU> textVerts;
	g_theFont->AddVertsForTextInBox2D(textVerts, AABB2(0.f, 400.f, 1600.f, 800.f), 40.f, "HIGH SCORE", Rgba8::COLOR_WHITE, 1.f, Vec2(0.5f, 0.5f));
	g_theFont->AddVertsForTextInBox2D(textVerts, AABB2(0.f, 200.f, 1600.f, 600.f), 120.f, Stringf("%02d", m_timeChallengeScore), Rgba8::COLOR_ORANGE, 1.f, Vec2(0.5f, 0.5f));
	g_theRenderer->BindTexture(&g_theFont->GetTexture());
	g_theRenderer->DrawVertexArray((int)textVerts.size(), textVerts.data());

	m_resultCanvas->Render();

	g_theRenderer->EndCamera(m_screenCamera);
}

//..............................
void Game::RenderScreenWorld() const
{
	g_theRenderer->BeginCamera(m_screenCamera);
	g_theRenderer->SetDepthStencilMode(DepthMode::DISABLED);
	g_theRenderer->SetBlendMode(BlendMode::ALPHA);
	g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_NONE);
	g_theRenderer->SetSamplerMode(SampleMode::POINT_CLAMP);

	AABB2 screenBound = AABB2(m_screenCamera.GetOrthographicBottomLeft(), m_screenCamera.GetOrthographicTopRight());

	std::vector<Vertex_PCU> UIVerts;
	UIVerts.reserve(6);

	if (m_clock->IsPaused())
	{
		AddVertsForAABB2D(UIVerts, screenBound, Rgba8::DARK);
	}

	g_theRenderer->SetModelConstants();
	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->DrawVertexArray((int)UIVerts.size(), UIVerts.data());

	g_theDevConsole->Render(screenBound, g_theRenderer);
}

void Game::ShakeCamera(float howStrong)
{
	if (!g_gameConfigBlackboard.GetValue("cameraShake", true))
	{
		return;
	}
	m_screenShakeAmount = howStrong;
}

void Game::InitMainMenuUI()
{
	m_menuCanvas = new Canvas(g_theUI, &m_screenCamera);

	// MAIN MENU
	m_mainPanel = new Panel(m_menuCanvas, AABB2(80, 80, 600, 370), Rgba8::COLOR_TRANSPARENT);
	m_mainPlayButton = new Button(m_menuCanvas, AABB2(100, 280, 300, 330), TextSetting("Play"), m_mainPanel);
	m_mainPlayButton->OnClickEvent([this]() {MainMenuButtonClickAction(MAIN_PLAY); });
	m_mainSettingButton = new Button(m_menuCanvas, AABB2(100, 200, 300, 250), TextSetting("Setting"), m_mainPanel);
	m_mainSettingButton->OnClickEvent([this]() {MainMenuButtonClickAction(MAIN_SETTING); });
	m_mainQuitButton = new Button(m_menuCanvas, AABB2(100, 120, 300, 170), TextSetting("Quit"), m_mainPanel);
	m_mainQuitButton->OnClickEvent([this]() {MainMenuButtonClickAction(MAIN_QUIT); });

	// GAME MODE
	m_gamemodePanel = new Panel(m_menuCanvas, AABB2(80, 80, 600, 370), Rgba8::COLOR_TRANSPARENT);
	m_gamemodePanel->SetActive(false);
	m_playCreativeButton = new Button(m_menuCanvas, AABB2(100, 320, 300, 370), TextSetting("Creative"), m_gamemodePanel);
	m_playCreativeButton->OnClickEvent([this]() {MainMenuButtonClickAction(PLAY_CREATIVE); });
	m_playTimerButton = new Button(m_menuCanvas, AABB2(100, 240, 300, 290), TextSetting("Timer"), m_gamemodePanel);
	m_playTimerButton->OnClickEvent([this]() {MainMenuButtonClickAction(PLAY_TIMER); });
	m_playObstacleButton = new Button(m_menuCanvas, AABB2(100, 160, 300, 210), TextSetting("Obstacle"), m_gamemodePanel);
	m_playObstacleButton->OnClickEvent([this]() {MainMenuButtonClickAction(PLAY_OBSTACLE); });
	m_playBackButton = new Button(m_menuCanvas, AABB2(100, 80, 300, 130), TextSetting("Back"), m_gamemodePanel);
	m_playBackButton->OnClickEvent([this]() {MainMenuButtonClickAction(BACK_TO_MENU); });

	// SETTING
	m_settingPanel = new Panel(m_menuCanvas, AABB2(80, 80, 600, 370), Rgba8::COLOR_TRANSPARENT);
	m_settingPanel->SetActive(false);
	m_settingMusicText = new Text(m_menuCanvas, Vec2(100, 295), TextSetting("Music", 20.f, Rgba8::COLOR_WHITE), m_settingPanel);
	m_settingSoundText = new Text(m_menuCanvas, Vec2(100, 245), TextSetting("Sound", 20.f, Rgba8::COLOR_WHITE), m_settingPanel);
	m_settingCameraShakeText = new Text(m_menuCanvas, Vec2(100, 195), TextSetting("Camera Shake", 20.f, Rgba8::COLOR_WHITE), m_settingPanel);
	m_settingMusicSlider = new Slider(m_menuCanvas, 0.f, MAX_VOLUME, AABB2(250, 295, 500, 315), Vec2(15, 15), m_settingPanel);
	m_settingMusicSlider->SetValue(g_gameConfigBlackboard.GetValue("music", 1.f));
	m_settingMusicSlider->OnChangingValueEvent([this]() {VolumeChangeAction(true); });
	m_settingSoundSlider = new Slider(m_menuCanvas, 0.f, MAX_VOLUME, AABB2(250, 245, 500, 265), Vec2(15, 15), m_settingPanel);
	m_settingSoundSlider->SetValue(g_gameConfigBlackboard.GetValue("sound", 1.f));
	m_settingSoundSlider->OnChangingValueEvent([this]() {VolumeChangeAction(false); });
	m_settingCameraShakeCheckbox = new Checkbox(m_menuCanvas, true, AABB2(370, 190, 400, 220), m_settingPanel);
	m_settingCameraShakeCheckbox->OnClickEvent([this]() {CheckboxClickAction(); });
	m_settingBackButton = new Button(m_menuCanvas, AABB2(100, 120, 300, 170), TextSetting("Back"), m_settingPanel);
	m_settingBackButton->OnClickEvent([this]() {MainMenuButtonClickAction(BACK_TO_MENU); });
}

void Game::InitTimerResultUI()
{
	m_resultCanvas = new Canvas(g_theUI, &m_screenCamera);

	m_resultBackToMenuButton = new Button(m_resultCanvas, AABB2(430, 50, 650, 120), TextSetting("Back To Menu"));
	m_resultBackToMenuButton->OnClickEvent([this]() {SwitchState(GameState::ATTRACT_MODE); });

	m_resultPlayAgainButton = new Button(m_resultCanvas, AABB2(950, 50, 1170, 120), TextSetting("Play Again"));
	m_resultPlayAgainButton->OnClickEvent([this]() {SwitchState(GameState::PLAY_MODE);});
}

void Game::EnterCurrentState()
{
	switch (m_currentState)
	{
	case GameState::ATTRACT_MODE:
		g_theInput->SetCursorMode(false, false);
		PlayMusic(m_menuMusic, true);
		break;
	case GameState::PLAY_MODE:
		g_theInput->SetCursorMode(true, true);
		m_map = new BasketballCourt();
		m_map->Startup();
		m_currentGameMusicIndex = g_theRNG->RollRandomIntInRange(0, 4);
		PlayMusic(m_gameMusics[m_currentGameMusicIndex], false);
		break;
	case GameState::RESULT_MODE:
		g_theInput->SetCursorMode(false, false);
		break;
	}
}

void Game::ExitCurrentState()
{
	switch (m_currentState)
	{
	case GameState::ATTRACT_MODE:

		break;
	case GameState::PLAY_MODE:
		if (m_currentGameMode == TIMER)
		{
			m_countdownReadyTimer = READY_TIMER;
			m_countdownTimeChallenge = TIME_CHALLENGE_TIMER;
		}
		if (m_currentGameMode == OBSTACLE)
		{
			
		}

		m_map->Shutdown();
		delete m_map;
		m_map = nullptr;
		break;
	case GameState::RESULT_MODE:
		break;
	}
}

void Game::SwitchState(GameState state)
{
	if (state == m_currentState)
	{
		return;
	}
	m_secondIntoMode = 0.f;
	ExitCurrentState();
	m_prevState = m_currentState;
	m_currentState = state;
	EnterCurrentState();
}

GameState Game::GetCurrentState() const
{
	return m_currentState;
}

void Game::PlaySoundUI(SoundID sound)
{
	m_currentSound = g_theAudio->StartSound(sound, false, g_gameConfigBlackboard.GetValue("sound", 1.f));
}

void Game::PlaySound(SoundID sound, bool overlapCurrentSound)
{
	float vol = g_gameConfigBlackboard.GetValue("sound", 1.f);
	if (g_gameConfigBlackboard.GetValue("debugMuteAll", false))
	{
		vol = 0.f;
	}
	if (overlapCurrentSound)
	{
		if (g_theAudio->IsPlaying(m_currentSound))
		{
			g_theAudio->StopSound(m_currentSound);
		}
		m_currentSound = g_theAudio->StartSound(sound, false, vol);
	}
	else
	{
		m_currentSound = g_theAudio->StartSound(sound, false, vol);
	}

}

void Game::PlayMusic(SoundID music, bool loop)
{
	if (g_theAudio->IsPlaying(m_currentMusic))
	{
		g_theAudio->StopSound(m_currentMusic);
	}
	float vol = g_gameConfigBlackboard.GetValue("music", 1.f);
	if (g_gameConfigBlackboard.GetValue("debugMuteAll", false))
	{
		vol = 0.f;
	}
	m_currentMusic = g_theAudio->StartSound(music, loop, vol);
}

void Game::GameMusicDJ()
{
	if (!g_theAudio->IsPlaying(m_currentMusic))
	{
		g_theAudio->StopSound(m_currentMusic);
		m_currentGameMusicIndex = g_theRNG->RollRandomIntInRange(0, 4);
		PlayMusic(m_gameMusics[m_currentGameMusicIndex], false);
	}
}

void Game::MainMenuButtonClickAction(MainMenuButtonOption option)
{
	PlaySoundUI(m_buttonClickSound);

	switch (option)
	{
	case MAIN_PLAY:
		m_mainPanel->SetActive(false);
		m_gamemodePanel->SetActive(true);
		break;
	case MAIN_SETTING:
		m_mainPanel->SetActive(false);
		m_settingPanel->SetActive(true);
		break;
	case MAIN_QUIT:
		FireEvent("quit");
		break;
	case PLAY_CREATIVE:
		m_currentGameMode = CREATIVE;
		SwitchState(GameState::PLAY_MODE);
		break;
	case PLAY_TIMER:
		m_currentGameMode = TIMER;
		SwitchState(GameState::PLAY_MODE);
		break;
	case PLAY_OBSTACLE:
		m_currentGameMode = OBSTACLE;
		SwitchState(GameState::PLAY_MODE);
		break;
	case BACK_TO_MENU:
		m_mainPanel->SetActive(true);
		m_gamemodePanel->SetActive(false);
		m_settingPanel->SetActive(false);
		break;
	}
}

void Game::VolumeChangeAction(bool isMusic)
{
	if (isMusic)
	{
		float music = MAX_VOLUME * m_settingMusicSlider->GetValueNormalized();
		g_gameConfigBlackboard.SetValue("music", std::to_string(music));
		g_theAudio->SetSoundPlaybackVolume(m_currentMusic, music);
	}
	else
	{
		float sound = MAX_VOLUME * m_settingSoundSlider->GetValueNormalized();
		g_gameConfigBlackboard.SetValue("sound", std::to_string(sound));
		g_theAudio->SetSoundPlaybackVolume(m_currentSound, sound);
	}
}

void Game::CheckboxClickAction()
{
	bool isCameraShake = g_gameConfigBlackboard.GetValue("cameraShake", true);
	g_gameConfigBlackboard.SetValue("cameraShake", std::to_string(!isCameraShake));
}
