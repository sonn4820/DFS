#pragma once
#include "Game/GameCommon.hpp"

class BasketballCourt;

class Entity
{
public:
	Entity(BasketballCourt* map);
	virtual ~Entity();
	
	virtual void Update(float deltaSeconds) = 0;
	virtual void UpdatePhysics(float fixedDeltaSeconds);
	virtual void Render() const = 0;

	void AddForce(Vec3 force);
	void AddImpulse(Vec3 impulse);

	void AddTorque(Vec3 pos, Vec3 force);
	void AddTorque(Vec3 torque);
	void AddImpulseTorque(Vec3 torque);
	void AddImpulseTorque(Vec3 pos, Vec3 force);

	Vec2 GetPositionXY() const;
	FloatRange GetHeightRange() const;
	virtual Mat44 GetModeMatrix() const;

public:
	BasketballCourt* m_map = nullptr;
	Vec3 m_position = Vec3::ZERO;
	Vec3 m_velocity = Vec3::ZERO;
	Vec3 m_acceleration = Vec3::ZERO;
	bool m_isGravityEnabled = true;
	bool m_isDead = false;
	bool m_isStatic = false;
	float m_drag = 9.f;
	float m_mass = 1.f;
	float m_radius = 1.f;
	float m_height = 1.f;
	EulerAngles m_orientationDegrees = EulerAngles();
	Vec3 m_angularVelocity = Vec3::ZERO;
	Vec3 m_angularAcceleration = Vec3::ZERO;
	Rgba8 m_color = Rgba8::COLOR_WHITE;
public:
};