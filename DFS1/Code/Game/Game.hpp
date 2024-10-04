#pragma once
#include "Game/GameCommon.hpp"

class BasketballCourt;

constexpr float TIME_CHALLENGE_TIMER = 120.f;
constexpr float READY_TIMER = 4.f;

enum class GameState
{
	NONE,
	ATTRACT_MODE,
	PLAY_MODE,
	RESULT_MODE
};

enum GameMode
{
	CREATIVE,
	TIMER,
	OBSTACLE,
};

enum MainMenuButtonOption
{
	MAIN_PLAY,
	MAIN_SETTING,
	MAIN_QUIT,
	PLAY_CREATIVE,
	PLAY_TIMER,
	PLAY_OBSTACLE,
	BACK_TO_MENU
};

class Game
{
public:
	Game();
	~Game();

	void Startup();
	void Update(float deltaSeconds);
	void UpdatePhysics(float fixedDeltaSeconds);
	void Render() const;
	void Shutdown();

	void LoadAttractData();
	void LoadGamePlayData();


	// STATE
	void EnterCurrentState();
	void ExitCurrentState();
	void SwitchState(GameState state);
	GameState GetCurrentState() const;

	// UPDATE
	void UpdatePlayMode(float deltaSeconds);
	void UpdateAttractMode(float deltaSeconds);
	void UpdateResultMode(float deltaSeconds);
	void UpdateCamera();

	// RENDER
	void RenderPlayMode() const;
	void RenderAttractMode() const;
	void RenderResultMode() const;
	void RenderScreenWorld() const;

	// UI
	void ShakeCamera(float howStrong);
	void InitMainMenuUI();
	void InitTimerResultUI();

	//SOUND
	void PlaySoundUI(SoundID sound);
	void PlaySound(SoundID sound, bool overlapCurrentSound);
	void PlayMusic(SoundID music, bool loop);
	void GameMusicDJ();

	void MainMenuButtonClickAction(MainMenuButtonOption option);
	void VolumeChangeAction(bool isMusic);
	void CheckboxClickAction();

public:
	int m_timeChallengeScore = 0;

	BasketballCourt* m_map = nullptr;

	Texture* m_menuTexture = nullptr;
	Texture* m_ballTexture = nullptr;
	Texture* m_courtTexture = nullptr;
	Texture* m_skyboxTexture = nullptr;
	Texture* m_netTexture = nullptr;
	Texture* m_hoopCylTexture = nullptr;
	Texture* m_hoopBoardTexture = nullptr;
	Texture* m_hoopBasketTexture = nullptr;
	Texture* m_crosshairTexture = nullptr;

	Texture* m_lebronBlockerTexture = nullptr;
	Texture* m_lukaBlockerTexture = nullptr;
	Texture* m_damianBlockerTexture = nullptr;

	SoundID m_menuMusic;
	SoundID* m_gameMusics = nullptr;

	SoundID m_buttonClickSound;
	SoundID m_sliderSound;
	SoundID m_ballBounceGroundSound;
	SoundID m_ballBounceBackboardSound;
	SoundID m_ballBounceNetSound;
	SoundID m_scoreSound;

	SoundID m_staticBlockerSound;
	SoundID m_continuousBlockerSound;
	SoundID m_timerBlockerSound;

	SoundPlaybackID m_currentMusic;
	SoundPlaybackID m_currentSound;

	Clock* m_clock = nullptr;
	float m_countdownTimeChallenge = TIME_CHALLENGE_TIMER;
	float m_countdownReadyTimer = READY_TIMER;
	
	GameState m_currentState = GameState::NONE;
	GameState m_prevState = GameState::NONE;

	GameMode m_currentGameMode = CREATIVE;

	// MENU
	Canvas* m_menuCanvas = nullptr;
	Panel* m_mainPanel = nullptr;
	Panel* m_gamemodePanel = nullptr;
	Panel* m_settingPanel = nullptr;
	Button* m_mainPlayButton = nullptr;
	Button* m_mainSettingButton = nullptr;
	Button* m_mainQuitButton = nullptr;
	Button* m_playCreativeButton = nullptr;
	Button* m_playTimerButton = nullptr;
	Button* m_playObstacleButton = nullptr;
	Button* m_playBackButton = nullptr;
	Text* m_settingMusicText = nullptr;
	Text* m_settingSoundText = nullptr;
	Text* m_settingCameraShakeText = nullptr;
	Slider* m_settingMusicSlider = nullptr;
	Slider* m_settingSoundSlider = nullptr;
	Checkbox* m_settingCameraShakeCheckbox = nullptr;
	Button* m_settingBackButton = nullptr;

	// TIMER RESULT
	Canvas* m_resultCanvas = nullptr;
	Button* m_resultPlayAgainButton = nullptr;
	Button* m_resultBackToMenuButton = nullptr;

	float m_screenShakeAmount = 0.0f;
	float m_secondIntoMode = 0.f;

	int m_currentGameMusicIndex = 0;
	int m_obstacleChallengeLevel = 1;

	Camera m_screenCamera;
	float m_fixedTimeStep = 0.005f;
	float m_timeDebt = 0.f;
};