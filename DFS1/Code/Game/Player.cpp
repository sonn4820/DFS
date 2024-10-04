#include "Player.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Prop.hpp"
#include "Game/Ball.hpp"
#include "Game/BasketballCourt.hpp"

bool g_gameplayMode = true;

Player::Player(BasketballCourt* map)
	:Entity(map)
{
	m_radius = 1.f;
	m_height = 6.5f;
	m_eyeHeight = 6.f;
	m_mass = 2.f;
	m_isGravityEnabled = true;
	m_playerCamera = new Camera();
	m_playerCamera->SetPerspectiveView(Window::GetMainWindowInstance()->GetAspect(), 60.f, 0.1f, 200.f);
	m_playerCamera->SetRenderBasis(Vec3(0.f, 0.f, 1.f), Vec3(-1.f, 0.f, 0.f), Vec3(0.f, 1.f, 0.f));
}

Player::~Player()
{
}

void Player::Update(float deltaSeconds)
{
	PlayerDataUpdateEveryFrame();

	HandleInput(deltaSeconds);

	DebugDataPrint();

	BallThrowingConstraint();
}

void Player::Render() const
{
	if (g_gameplayMode)
	{
		return;
	}

	std::vector<Vertex_PCU> verts;

	AddVertsForZCylinder3D(verts, Vec2(0, 0), FloatRange(0, m_height), m_radius, 16);

	g_theRenderer->BindShader(nullptr);
	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->SetModelConstants(GetModelMatrix(), m_color);
	g_theRenderer->SetDepthStencilMode(DepthMode::ENABLED);
	g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
	g_theRenderer->DrawVertexArray((int)verts.size(), verts.data());
}

void Player::RenderUI() const
{
	g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_NONE);
	g_theRenderer->BindShader(nullptr);
	g_theRenderer->SetModelConstants();

	std::vector<Vertex_PCU> textVerts;

	if (g_gameplayMode)
	{
		std::vector<Vertex_PCU> UIShape;
		Vec2 UISpinCenter = Vec2(200, 120);
		float UISpinRadius = 80.f;
		AddVertsForDisc2DGradient(UIShape, UISpinCenter, UISpinRadius, Rgba8::COLOR_WHITE, Rgba8::COLOR_ORANGE);
		float spinX = RangeMapClamped(m_spinPosition.x, -BALL_RADIUS, BALL_RADIUS, -1.f, 1.f);
		float spinY = RangeMapClamped(m_spinPosition.y, -BALL_RADIUS, BALL_RADIUS, -1.f, 1.f);
		Vec2 SpinCursosPos = UISpinCenter + (Vec2(spinX, spinY) * UISpinRadius).GetClamped(UISpinRadius);
		AddVertsForRing2D(UIShape, SpinCursosPos, 7, 4, Rgba8::COLOR_RED);

		AddVertsForAABB2D(UIShape, AABB2(40, 40, 80, 300), Rgba8::COLOR_DARK_GRAY);
		float forceZeroToOne = RangeMapClamped(m_throwForce, 0.f, THROW_MAX_FORCE, 0.f, 1.f);
		float forceHeight = RangeMapClamped(forceZeroToOne, 0.f, 1.f, 40.f, 300.f);
		Rgba8 forceColor = Interpolate(Rgba8::COLOR_GREEN, Rgba8::COLOR_RED, forceZeroToOne);
		AddVertsForAABB2D(UIShape, AABB2(40, 40, 80, forceHeight), forceColor);
		g_theRenderer->BindTexture(nullptr);
		g_theRenderer->DrawVertexArray((int)UIShape.size(), UIShape.data());

		g_theFont->AddVertsForText2D(textVerts, Vec2(190, 180), 15.f, "T", Rgba8::COLOR_VIOLET);
		g_theFont->AddVertsForText2D(textVerts, Vec2(190, 40), 15.f, "B", Rgba8::COLOR_VIOLET);
		g_theFont->AddVertsForText2D(textVerts, Vec2(125, 110), 15.f, "L", Rgba8::COLOR_VIOLET);
		g_theFont->AddVertsForText2D(textVerts, Vec2(260, 112), 15.f, "R", Rgba8::COLOR_VIOLET);
		g_theFont->AddVertsForText2D(textVerts, Vec2(160, 5), 20.f, "SPIN", Rgba8::COLOR_YELLOW);
		g_theFont->AddVertsForText2D(textVerts, Vec2(15, 5), 20.f, "FORCE", Rgba8::COLOR_YELLOW);

		float angle = RangeMapClamped(m_throwDirection.z, 0.f, 1.f, -30.f, 70.f);
		Rgba8 angleColor = Interpolate(Rgba8::COLOR_CYAN, Rgba8::COLOR_GREEN, m_throwDirection.z);
		g_theFont->AddVertsForTextInBox2D(textVerts, AABB2(760, 360, 840, 380), 20.f, Stringf("%02d", RoundDownToInt(angle)), angleColor);
		g_theFont->AddVertsForTextInBox2D(textVerts, AABB2(760, 340, 840, 355), 15.f, "ANGLE", Rgba8::COLOR_YELLOW);
	}

	// INSTRUCTION
	g_theFont->AddVertsForTextInBox2D(textVerts, AABB2(1300.f, 175.f, 1580.f, 195.f), 20.f, "Instruction:", Rgba8::COLOR_ORANGE, 1.f, Vec2(1.f, 0.5f));

	g_theFont->AddVertsForTextInBox2D(textVerts, AABB2(800.f, 155.f, 1580.f, 165.f), 10.f, "WASD to move", Rgba8::COLOR_YELLOW, 1.f, Vec2(1.f, 0.5f));

	if (g_gameplayMode)
	{
		g_theFont->AddVertsForTextInBox2D(textVerts, AABB2(800.f, 140.f, 1580.f, 150.f), 10.f, "Space to jump", Rgba8::COLOR_YELLOW, 1.f, Vec2(1.f, 0.5f));
		g_theFont->AddVertsForTextInBox2D(textVerts, AABB2(800.f, 125.f, 1580.f, 135.f), 10.f, "Hold Crtl and use mouse to adjust spin", Rgba8::COLOR_YELLOW, 1.f, Vec2(1.f, 0.5f));
		g_theFont->AddVertsForTextInBox2D(textVerts, AABB2(800.f, 110.f, 1580.f, 120.f), 10.f, "Mouse wheel to change value", Rgba8::COLOR_YELLOW, 1.f, Vec2(1.f, 0.5f));
		g_theFont->AddVertsForTextInBox2D(textVerts, AABB2(800.f, 95.f, 1580.f, 105.f), 10.f, "LMB to shoot (hold/release)", Rgba8::COLOR_YELLOW, 1.f, Vec2(1.f, 0.5f));
		g_theFont->AddVertsForTextInBox2D(textVerts, AABB2(800.f, 80.f, 1580.f, 90.f), 10.f, "RMB to Dribble", Rgba8::COLOR_YELLOW, 1.f, Vec2(1.f, 0.5f));
		g_theFont->AddVertsForTextInBox2D(textVerts, AABB2(800.f, 65.f, 1580.f, 75.f), 10.f, "F to pick up ball", Rgba8::COLOR_YELLOW, 1.f, Vec2(1.f, 0.5f));
		g_theFont->AddVertsForTextInBox2D(textVerts, AABB2(800.f, 50.f, 1580.f, 60.f), 10.f, "N to spawn ball", Rgba8::COLOR_YELLOW, 1.f, Vec2(1.f, 0.5f));
	}
	else
	{
		g_theFont->AddVertsForTextInBox2D(textVerts, AABB2(800.f, 125.f, 1580.f, 135.f), 10.f, "Q/E to change option", Rgba8::COLOR_YELLOW, 1.f, Vec2(1.f, 0.5f));
		g_theFont->AddVertsForTextInBox2D(textVerts, AABB2(800.f, 110.f, 1580.f, 120.f), 10.f, "Mouse wheel to change value", Rgba8::COLOR_YELLOW, 1.f, Vec2(1.f, 0.5f));
		g_theFont->AddVertsForTextInBox2D(textVerts, AABB2(800.f, 95.f, 1580.f, 105.f), 10.f, "LMB to shoot", Rgba8::COLOR_YELLOW, 1.f, Vec2(1.f, 0.5f));
		g_theFont->AddVertsForTextInBox2D(textVerts, AABB2(800.f, 65.f, 1580.f, 75.f), 10.f, "C to clear all balls", Rgba8::COLOR_YELLOW, 1.f, Vec2(1.f, 0.5f));
		g_theFont->AddVertsForTextInBox2D(textVerts, AABB2(800.f, 50.f, 1580.f, 60.f), 10.f, "N / M to spawn ball", Rgba8::COLOR_YELLOW, 1.f, Vec2(1.f, 0.5f));
	}
	g_theFont->AddVertsForTextInBox2D(textVerts, AABB2(800.f, 35.f, 1580.f, 45.f), 10.f, "K/L to change music", Rgba8::COLOR_YELLOW, 1.f, Vec2(1.f, 0.5f));
	g_theFont->AddVertsForTextInBox2D(textVerts, AABB2(800.f, 5.f, 1580.f, 20.f), 10.f, "F1: DEBUG DRAW / F2: DEBUG MODE", Rgba8::COLOR_WHITE, 1.f, Vec2(1.f, 0.5f));
	g_theRenderer->BindTexture(&g_theFont->GetTexture());
	g_theRenderer->DrawVertexArray((int)textVerts.size(), textVerts.data());

	std::vector<Vertex_PCU> reticle;
	Vec2 center = Vec2(g_theGame->m_screenCamera.GetOrthographicTopRight().x * 0.5f, g_theGame->m_screenCamera.GetOrthographicBottomLeft().y + (g_theGame->m_screenCamera.GetOrthographicTopRight().y - g_theGame->m_screenCamera.GetOrthographicBottomLeft().y) * 0.5f);
	IntVec2 reticleHalfSize = IntVec2(10, 10);
	AABB2 reticleBound = AABB2(center - Vec2((float)reticleHalfSize.x, (float)reticleHalfSize.y), center + Vec2((float)reticleHalfSize.x, (float)reticleHalfSize.y));
	AddVertsForAABB2D(reticle, reticleBound, Rgba8::COLOR_WHITE);
	g_theRenderer->BindTexture(g_theGame->m_crosshairTexture);
	g_theRenderer->DrawVertexArray((int)reticle.size(), reticle.data());

	if (g_theGame->m_currentGameMode == TIMER)
	{
		g_theFont->AddVertsForTextInBox2D(textVerts, AABB2(1200.f, 700.f, 1580.f, 720.f), 20.f, "Time Left", Rgba8::COLOR_WHITE);
		if (g_theGame->m_countdownTimeChallenge > 10.f)
		{
			g_theFont->AddVertsForTextInBox2D(textVerts, AABB2(1200.f, 630.f, 1580.f, 680.f), 40.f, Stringf("%02d", RoundDownToInt(g_theGame->m_countdownTimeChallenge)), Rgba8::COLOR_BLACK);
		}
		else
		{
			g_theFont->AddVertsForTextInBox2D(textVerts, AABB2(1200.f, 630.f, 1580.f, 680.f), 40.f, Stringf("%02d", RoundDownToInt(g_theGame->m_countdownTimeChallenge)), Rgba8::COLOR_RED);
		}

		if (g_theGame->m_countdownReadyTimer > 0.f)
		{
			g_theFont->AddVertsForTextInBox2D(textVerts, AABB2(0.f, 620.f, 1600.f, 800.f), 40.f, "Ready In", Rgba8::COLOR_WHITE);
			g_theFont->AddVertsForTextInBox2D(textVerts, AABB2(0.f, 520.f, 1600.f, 650.f), 70.f, Stringf("%02d", RoundDownToInt(g_theGame->m_countdownReadyTimer)), Rgba8::COLOR_YELLOW);
		}
		else
		{
			g_theFont->AddVertsForTextInBox2D(textVerts, AABB2(0.f, 720.f, 1600.f, 800.f), 40.f, "Score", Rgba8::COLOR_WHITE);
			g_theFont->AddVertsForTextInBox2D(textVerts, AABB2(0.f, 650.f, 1600.f, 750.f), 50.f, Stringf("%02d", g_theGame->m_timeChallengeScore), Rgba8::COLOR_ORANGE);
		}
		g_theRenderer->BindTexture(&g_theFont->GetTexture());
		g_theRenderer->DrawVertexArray((int)textVerts.size(), textVerts.data());
	}

	if (g_theGame->m_currentGameMode == OBSTACLE)
	{
		g_theFont->AddVertsForTextInBox2D(textVerts, AABB2(1200.f, 700.f, 1580.f, 720.f), 20.f, "Current Level", Rgba8::COLOR_WHITE);
		g_theFont->AddVertsForTextInBox2D(textVerts, AABB2(1200.f, 630.f, 1580.f, 680.f), 40.f, Stringf("%02d", g_theGame->m_obstacleChallengeLevel), Rgba8::COLOR_BLACK);

		g_theRenderer->BindTexture(&g_theFont->GetTexture());
		g_theRenderer->DrawVertexArray((int)textVerts.size(), textVerts.data());
	}
}

Camera* Player::GetCamera()
{
	return m_playerCamera;
}

void Player::HandleInput(float deltaSeconds)
{
	UNUSED(deltaSeconds);


	if (g_gameplayMode)
	{
		GameplayOptionControl();
	}
	else
	{
		DebugOptionControl();
	}
}

void Player::PlayerDataUpdateEveryFrame()
{
	if (g_gameplayMode)
	{
		m_orientationDegrees.m_pitchDegrees = Clamp(m_orientationDegrees.m_pitchDegrees, -85.f, 85.f);
		m_playerCamera->SetTransform(Vec3(m_position.x, m_position.y, m_position.z + m_eyeHeight), m_orientationDegrees);
		m_refPos = Vec3(m_position.x, m_position.y, m_position.z + m_eyeHeight * 0.9f) + 1.2f * m_playerCamera->GetModelMatrix().GetIBasis3D().GetNormalized();
		if (RaycastVsPlane3D(m_position, Vec3(0, 0, -1), 0.2f, m_map->m_gameFloor).m_didImpact)
		{
			m_isJumped = false;
		}
		if (m_isHoldingBall && m_currentBall)
		{
			m_currentBall->m_position = m_refPos;
			m_currentBall->m_rotation = Quaternion(EulerAngles(m_orientationDegrees.m_yawDegrees, m_orientationDegrees.m_pitchDegrees, 90.f));
		}
		if (g_theGame->m_currentGameMode == TIMER)
		{
			if (!m_isHoldingBall)
			{
				m_timeSinceThrowBall += g_theGame->m_clock->GetDeltaSeconds();
				if (m_timeSinceThrowBall > 0.5f && !m_currentBall)
				{
					m_currentBall = m_map->CreateBall(m_position);
					m_isHoldingBall = true;
					m_currentBall->m_isSimulatingPhysics = false;
				}
			}
			else
			{
				m_timeSinceThrowBall = 0.f;
			}
		}
	}
	else
	{
		m_orientationDegrees.m_pitchDegrees = 0;
	}
}

void Player::DebugDataPrint()
{
	DebugAddScreenText(Stringf("[Game Clock] Time: %.1f, FPS: %.1f, Scale: %.1f", g_theGame->m_clock->GetTotalSeconds(), 1.f / g_theGame->m_clock->GetDeltaSeconds(), g_theGame->m_clock->GetTimeScale()),
		Vec2(1010.f, 780.f), 12.5f);

	if (!g_gameplayMode)
	{
		DebugAddScreenText(Stringf("Debug Mode"), Vec2(20.f, 770.f), 20.f, Rgba8::COLOR_DARK_BLUE);

		if (m_currentDebugOption == FORCE)
		{
			DebugAddScreenText(Stringf("Throw Force: %.f", m_throwForce), Vec2(20.f, 710.f), 20.f, Rgba8::COLOR_GREEN);
		}
		else
		{
			DebugAddScreenText(Stringf("Throw Force: %.f", m_throwForce), Vec2(20.f, 710.f), 20.f, Rgba8::COLOR_DARK_GRAY);
		}
		if (m_currentDebugOption == DIRECTION_X)
		{
			DebugAddScreenText(Stringf("Throw Direction X: %.2f", m_throwDirection.x), Vec2(20.f, 680.f), 20.f, Rgba8::COLOR_GREEN);
		}
		else
		{
			DebugAddScreenText(Stringf("Throw Direction X: %.2f", m_throwDirection.x), Vec2(20.f, 680.f), 20.f, Rgba8::COLOR_DARK_GRAY);
		}
		if (m_currentDebugOption == DIRECTION_Y)
		{
			DebugAddScreenText(Stringf("Throw Direction Y: %.2f", m_throwDirection.y), Vec2(20.f, 650.f), 20.f, Rgba8::COLOR_GREEN);
		}
		else
		{
			DebugAddScreenText(Stringf("Throw Direction Y: %.2f", m_throwDirection.y), Vec2(20.f, 650.f), 20.f, Rgba8::COLOR_DARK_GRAY);
		}
		if (m_currentDebugOption == DIRECTION_Z)
		{
			DebugAddScreenText(Stringf("Throw Direction Z: %.2f", m_throwDirection.z), Vec2(20.f, 620.f), 20.f, Rgba8::COLOR_GREEN);
		}
		else
		{
			DebugAddScreenText(Stringf("Throw Direction Z: %.2f", m_throwDirection.z), Vec2(20.f, 620.f), 20.f, Rgba8::COLOR_DARK_GRAY);
		}
		if (m_currentDebugOption == SPIN_POS_X)
		{
			DebugAddScreenText(Stringf("Spin Position X: %.2f", m_spinPosition.x), Vec2(20.f, 590.f), 20.f, Rgba8::COLOR_GREEN);
		}
		else
		{
			DebugAddScreenText(Stringf("Spin Position X: %.2f", m_spinPosition.x), Vec2(20.f, 590.f), 20.f, Rgba8::COLOR_DARK_GRAY);
		}
		if (m_currentDebugOption == SPIN_POS_Y)
		{
			DebugAddScreenText(Stringf("Spin Position Y: %.2f", m_spinPosition.y), Vec2(20.f, 560.f), 20.f, Rgba8::COLOR_GREEN);
		}
		else
		{
			DebugAddScreenText(Stringf("Spin Position Y: %.2f", m_spinPosition.y), Vec2(20.f, 560.f), 20.f, Rgba8::COLOR_DARK_GRAY);
		}
		if (m_currentDebugOption == SPIN_POS_Z)
		{
			DebugAddScreenText(Stringf("Spin Position Z: %.2f", m_spinPosition.z), Vec2(20.f, 530.f), 20.f, Rgba8::COLOR_GREEN);
		}
		else
		{
			DebugAddScreenText(Stringf("Spin Position Z: %.2f", m_spinPosition.z), Vec2(20.f, 530.f), 20.f, Rgba8::COLOR_DARK_GRAY);
		}
		if (m_currentDebugOption == SPIN_FORCE_X)
		{
			DebugAddScreenText(Stringf("Spin Force X: %.f", m_spinForce.x), Vec2(20.f, 500.f), 20.f, Rgba8::COLOR_GREEN);
		}
		else
		{
			DebugAddScreenText(Stringf("Spin Force X: %.f", m_spinForce.x), Vec2(20.f, 500.f), 20.f, Rgba8::COLOR_DARK_GRAY);
		}
		if (m_currentDebugOption == SPIN_FORCE_Y)
		{
			DebugAddScreenText(Stringf("Spin Force Y: %.f", m_spinForce.y), Vec2(20.f, 470.f), 20.f, Rgba8::COLOR_GREEN);
		}
		else
		{
			DebugAddScreenText(Stringf("Spin Force Y: %.f", m_spinForce.y), Vec2(20.f, 470.f), 20.f, Rgba8::COLOR_DARK_GRAY);
		}
		if (m_currentDebugOption == SPIN_FORCE_Z)
		{
			DebugAddScreenText(Stringf("Spin Force Z: %.f", m_spinForce.z), Vec2(20.f, 440.f), 20.f, Rgba8::COLOR_GREEN);
		}
		else
		{
			DebugAddScreenText(Stringf("Spin Force Z: %.f", m_spinForce.z), Vec2(20.f, 440.f), 20.f, Rgba8::COLOR_DARK_GRAY);
		}

		DebugAddScreenText(Stringf("Total balls : %i", m_map->m_ballList.size()), Vec2(20, 20), 20);
	}
	else
	{
		DebugAddScreenText(Stringf("Gameplay Mode"), Vec2(20.f, 770.f), 20.f, Rgba8::COLOR_RED);
	}
}

void Player::BallThrowingConstraint()
{
	m_throwDirection.x = Clamp(m_throwDirection.x, -1.f, 1.f);
	m_throwDirection.y = Clamp(m_throwDirection.y, -1.f, 1.f);
	m_throwDirection.z = Clamp(m_throwDirection.z, 0.f, 1.f);

	m_spinPosition.x = Clamp(m_spinPosition.x, -BALL_RADIUS, BALL_RADIUS);
	m_spinPosition.y = Clamp(m_spinPosition.y, -BALL_RADIUS, BALL_RADIUS);
	m_spinPosition.z = Clamp(m_spinPosition.z, -BALL_RADIUS, BALL_RADIUS);

	m_throwForce = Clamp(m_throwForce, 0.f, THROW_MAX_FORCE);
	m_spinForce.x = Clamp(m_spinForce.x, 0.f, SPIN_MAX_FORCE);
	m_spinForce.y = Clamp(m_spinForce.y, 0.f, SPIN_MAX_FORCE);
	m_spinForce.z = Clamp(m_spinForce.z, 0.f, SPIN_MAX_FORCE);
}

void Player::Shoot()
{
	if (!m_currentBall)
	{
		return;
	}
	m_currentBall->m_isSimulatingPhysics = true;
	m_currentBall->AddImpulse(m_throwDirection * m_throwForce);
	m_currentBall->AddTorque(m_currentBall->m_position - GetModelMatrix().GetIBasis3D(), Vec3(0.f, -m_spinPosition.x, m_spinPosition.y) * m_spinForce);
	if (g_gameplayMode)
	{
		m_isHoldingBall = false;
		m_currentBall = nullptr;
	}
}

void Player::Dribble()
{
	if (!m_currentBall)
	{
		return;
	}
	m_currentBall->m_isSimulatingPhysics = true;
	float power = Vec2(m_velocity.x, m_velocity.y).GetLength() * 0.1f;
	m_currentBall->AddImpulse(Vec3(GetModelMatrix().GetIBasis3D().x * power, GetModelMatrix().GetIBasis3D().y * power, -1) * 1700.f);
	Vec3 spin = Vec3(g_theRNG->RollRandomFloatMinusOneToOne(), g_theRNG->RollRandomFloatMinusOneToOne(), g_theRNG->RollRandomFloatMinusOneToOne());
	m_currentBall->AddImpulseTorque(spin * 1.5f);
	m_isHoldingBall = false;
	m_currentBall = nullptr;
}

void Player::PickUpBallWithRaycast()
{
	GameRaycast3D ray = m_map->RaycastVsBall(m_refPos, GetModelMatrix().GetIBasis3D());
	if (!m_currentBall)
	{
		if (ray.m_didImpact)
		{
			if (g_theInput->WasKeyJustPressed(KEYCODE_RIGHT_MOUSE) || g_theInput->WasKeyJustPressed('F'))
			{
				m_currentBall = ray.m_hitBall;
				m_isHoldingBall = true;
				m_currentBall->m_isSimulatingPhysics = false;
			}
		}
	}
}

void Player::AdjustAngle()
{
	if (g_theInput->WasMouseWheelScrolledUp())
	{
		m_throwDirection.z += DIR_AND_POS_RATE * 2.f;
	}
	if (g_theInput->WasMouseWheelScrolledDown())
	{
		m_throwDirection.z -= DIR_AND_POS_RATE * 2.f;
	}
}

void Player::AdjustForce()
{
	if (!m_currentBall)
	{
		return;
	}
	m_throwForce += FORCE_RATE;
}

void Player::DebugOptionControl()
{
	if (g_theInput->WasKeyJustPressed(KEYCODE_LEFT_MOUSE) && m_currentBall)
	{
		Shoot();
	}
	if (g_theInput->WasKeyJustPressed('C'))
	{
		m_map->DeleteAllBalls();
	}
	if (g_theInput->WasKeyJustPressed('N'))
	{
		m_currentBall = m_map->CreateBall(Vec3(0, 0, 2));
	}
	if (g_theInput->WasMouseWheelScrolledDown())
	{
		switch (m_currentDebugOption)
		{
		case FORCE:
			m_throwForce -= FORCE_RATE;
			break;
		case DIRECTION_X:
			m_throwDirection.x -= DIR_AND_POS_RATE;
			break;
		case DIRECTION_Y:
			m_throwDirection.y -= DIR_AND_POS_RATE;
			break;
		case DIRECTION_Z:
			m_throwDirection.z -= DIR_AND_POS_RATE;
			break;
		case SPIN_POS_X:
			m_spinPosition.x -= DIR_AND_POS_RATE;
			break;
		case SPIN_POS_Y:
			m_spinPosition.y -= DIR_AND_POS_RATE;
			break;
		case SPIN_POS_Z:
			m_spinPosition.z -= DIR_AND_POS_RATE;
			break;
		case SPIN_FORCE_X:
			m_spinForce.x -= SPIN_RATE;
			break;
		case SPIN_FORCE_Y:
			m_spinForce.y -= SPIN_RATE;
			break;
		case SPIN_FORCE_Z:
			m_spinForce.z -= SPIN_RATE;
			break;
		}
	}
	if (g_theInput->WasMouseWheelScrolledUp())
	{
		switch (m_currentDebugOption)
		{
		case FORCE:
			m_throwForce += FORCE_RATE;
			break;
		case DIRECTION_X:
			m_throwDirection.x += DIR_AND_POS_RATE;
			break;
		case DIRECTION_Y:
			m_throwDirection.y += DIR_AND_POS_RATE;
			break;
		case DIRECTION_Z:
			m_throwDirection.z += DIR_AND_POS_RATE;
			break;
		case SPIN_POS_X:
			m_spinPosition.x += DIR_AND_POS_RATE;
			break;
		case SPIN_POS_Y:
			m_spinPosition.y += DIR_AND_POS_RATE;
			break;
		case SPIN_POS_Z:
			m_spinPosition.z += DIR_AND_POS_RATE;
			break;
		case SPIN_FORCE_X:
			m_spinForce.x += SPIN_RATE;
			break;
		case SPIN_FORCE_Y:
			m_spinForce.y += SPIN_RATE;
			break;
		case SPIN_FORCE_Z:
			m_spinForce.z += SPIN_RATE;
			break;
		}
	}

	if (g_theInput->WasKeyJustPressed('Q') || g_theInput->WasKeyJustPressed(KEYCODE_UPARROW))
	{
		if (m_currentDebugOption == FORCE)
		{
			m_currentDebugOption = SPIN_FORCE_Z;
		}
		else
		{
			m_currentDebugOption = (BallDebugOption)(m_currentDebugOption - 1);
		}
	}
	if (g_theInput->WasKeyJustPressed('E') || g_theInput->WasKeyJustPressed(KEYCODE_DOWNARROW))
	{
		if (m_currentDebugOption == SPIN_FORCE_Z)
		{
			m_currentDebugOption = FORCE;
		}
		else
		{
			m_currentDebugOption = (BallDebugOption)(m_currentDebugOption + 1);
		}
	}

	if (g_theInput->WasKeyJustPressed('R'))
	{
		m_throwForce = 0.f;
		m_throwDirection = Vec3();
		m_spinPosition = Vec3();
		m_spinForce = Vec3();
	}

	Vec3 forwardDir = m_playerCamera->GetModelMatrix().GetIBasis3D().GetNormalized();
	Vec3 rightDir = m_playerCamera->GetModelMatrix().GetJBasis3D().GetNormalized();
	Vec3 upDir = m_playerCamera->GetModelMatrix().GetKBasis3D().GetNormalized();

	float walk = 5.f;
	float run = 20.f;
	Vec3 velocity = Vec3::ZERO;

	// MKB
	m_playerCamera->m_orientation.m_pitchDegrees += g_theInput->GetCursorClientDelta().y * 0.075f;
	m_playerCamera->m_orientation.m_yawDegrees -= g_theInput->GetCursorClientDelta().x * 0.075f;


	if (g_theInput->IsKeyDown('W'))
	{
		velocity += forwardDir * walk;
	}
	if (g_theInput->IsKeyDown('S'))
	{
		velocity -= forwardDir * walk;
	}
	if (g_theInput->IsKeyDown('A'))
	{
		velocity += rightDir * walk;
	}
	if (g_theInput->IsKeyDown('D'))
	{
		velocity -= rightDir * walk;
	}
	if (g_theInput->IsKeyDown(KEYCODE_SHIFT))
	{
		velocity *= run;
	}

	m_playerCamera->m_orientation.m_pitchDegrees = Clamp(m_playerCamera->m_orientation.m_pitchDegrees, -85.f, 85.f);
	m_playerCamera->m_position += velocity * g_theGame->m_clock->GetDeltaSeconds();

	if (g_theInput->WasKeyJustPressed('M'))
	{
		for (size_t i = 0; i < 50; i++)
		{
			float randXPos = g_theRNG->RollRandomFloatInRange(-10.f, 10.f);
			float randYPos = g_theRNG->RollRandomFloatInRange(-10.f, 10.f);
			float randZPos = g_theRNG->RollRandomFloatInRange(25.f, 30.f);
			Ball* ball = m_map->CreateBall(Vec3(randXPos, randYPos, randZPos));

			float randXVel = g_theRNG->RollRandomFloatInRange(-15.f, 15.f);
			float randYVel = g_theRNG->RollRandomFloatInRange(-15.f, 15.f);
			float randZVel = g_theRNG->RollRandomFloatInRange(-15.f, 15.f);
			ball->AddImpulse(Vec3(randXVel, randYVel, randZVel));
		}
	}
}

void Player::GameplayOptionControl()
{
	m_throwDirection = Vec3(GetModelMatrix().GetIBasis3D().x, GetModelMatrix().GetIBasis3D().y, m_throwDirection.z);

	PickUpBallWithRaycast();

	if (g_theGame->m_currentGameMode == CREATIVE || g_theGame->m_currentGameMode == OBSTACLE)
	{
		if (g_theInput->WasKeyJustPressed('N'))
		{
			if (!m_currentBall)
			{
				m_currentBall = m_map->CreateBall(m_position);
				m_isHoldingBall = true;
			}
		}

		if (g_theInput->IsKeyDown(KEYCODE_LEFT_MOUSE))
		{
			AdjustForce();
		}
		else
		{
			if (g_theInput->WasKeyJustPressed(KEYCODE_RIGHT_MOUSE) && m_currentBall)
			{
				Dribble();
			}
			if (g_theInput->WasKeyJustReleased(KEYCODE_LEFT_MOUSE) && m_currentBall)
			{
				Shoot();
			}
			m_throwForce = 0;
		}
	}

	if (g_theGame->m_currentGameMode == TIMER && g_theGame->m_countdownReadyTimer < 0.f)
	{
		if (g_theInput->IsKeyDown(KEYCODE_LEFT_MOUSE))
		{
			AdjustForce();
		}
		else
		{
			if (g_theInput->WasKeyJustPressed(KEYCODE_RIGHT_MOUSE) && m_currentBall)
			{
				Dribble();
			}
			if (g_theInput->WasKeyJustReleased(KEYCODE_LEFT_MOUSE) && m_currentBall)
			{
				Shoot();
			}
			m_throwForce = 0;
		}
	}

	AdjustAngle();

	if (g_theInput->IsKeyDown(KEYCODE_CTRL))
	{
		m_spinPosition.x += g_theInput->GetCursorClientDelta().x * 0.0075f;
		m_spinPosition.y -= g_theInput->GetCursorClientDelta().y * 0.0075f;
		float force = RangeMapClamped(Vec2(m_spinPosition.x, m_spinPosition.y).GetLength(), 0.f, 1.f, 0.f, 20000.f);
		m_spinForce = Vec3(force, force, force);
	}
	else
	{
		m_orientationDegrees.m_pitchDegrees += g_theInput->GetCursorClientDelta().y * 0.075f;
		m_orientationDegrees.m_yawDegrees -= g_theInput->GetCursorClientDelta().x * 0.075f;
	}


	if (g_theGame->m_currentGameMode != TIMER)
	{
		Vec3 forwardDir = GetModelMatrix().GetIBasis3D().GetNormalized();
		Vec3 rightDir = GetModelMatrix().GetJBasis3D().GetNormalized();
		Vec3 upDir = GetModelMatrix().GetKBasis3D().GetNormalized();

		if (g_theInput->IsKeyDown('W'))
		{
			AddForce(Vec3(forwardDir.x, forwardDir.y, 0.f).GetNormalized() * 9.f * m_movingSpeed);
		}
		if (g_theInput->IsKeyDown('S'))
		{
			AddForce(-Vec3(forwardDir.x, forwardDir.y, 0.f).GetNormalized() * 9.f * m_movingSpeed);
		}
		if (g_theInput->IsKeyDown('A'))
		{
			AddForce(Vec3(rightDir.x, rightDir.y, 0.f).GetNormalized() * 9.f * m_movingSpeed);
		}
		if (g_theInput->IsKeyDown('D'))
		{
			AddForce(-Vec3(rightDir.x, rightDir.y, 0.f).GetNormalized() * 9.f * m_movingSpeed);
		}
		if (g_theInput->WasKeyJustPressed(KEYCODE_SPACE) && !m_isJumped)
		{
			AddImpulse(Vec3(0, 0, 1) * 50.f);
			m_isJumped = true;
		}
		if (g_theInput->IsKeyDown(KEYCODE_SHIFT))
		{
			m_movingSpeed = 75.f;
		}
		else
		{
			m_movingSpeed = 25.f;
		}
	}
}

Mat44 Player::GetModelMatrix() const
{
	Mat44 modelMat = Mat44();
	modelMat = m_orientationDegrees.GetAsMatrix_IFwd_JLeft_KUp();
	modelMat.SetTranslation3D(m_position);
	return modelMat;
}