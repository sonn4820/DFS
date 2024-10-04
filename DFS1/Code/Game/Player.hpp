#pragma once
#include "Game/GameCommon.hpp"
#include "Game/Entity.hpp"

class Prop;
class Ball;

enum BallDebugOption
{
	FORCE,
	DIRECTION_X,
	DIRECTION_Y,
	DIRECTION_Z,
	SPIN_POS_X,
	SPIN_POS_Y,
	SPIN_POS_Z,
	SPIN_FORCE_X,
	SPIN_FORCE_Y,
	SPIN_FORCE_Z,
};

class Player : public Entity
{
public:
	Player(BasketballCourt* map);
	virtual ~Player();

	virtual void Update(float deltaSeconds) override;
	virtual void Render() const override;
	void RenderUI() const;

	Camera* GetCamera();
	Mat44 GetModelMatrix() const;
public:
	Ball* m_currentBall = nullptr;
	float m_throwForce = 500.f;
	Vec3 m_throwDirection = Vec3(0, 0, 0.7f);
	float m_eyeHeight = 5.f;
	Vec3 m_spinPosition;
	Vec3 m_spinForce;
	bool m_isHoldingBall = false;

	float m_timeSinceThrowBall = 0.2f;

	Camera* m_playerCamera = nullptr;
	float m_movingSpeed = 10.f;
	float m_rotatingSpeed = 90.f;
	Vec3 m_refPos;
	bool m_isJumped = false;

	BallDebugOption m_currentDebugOption = FORCE;
private:
	void HandleInput(float deltaSeconds);
	void PlayerDataUpdateEveryFrame();
	void DebugDataPrint();

	void BallThrowingConstraint();
	void Shoot();
	void Dribble();
	void PickUpBallWithRaycast();
	void AdjustForce();
	void AdjustAngle();

	void DebugOptionControl();
	void GameplayOptionControl();
};