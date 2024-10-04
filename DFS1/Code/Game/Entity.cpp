#include "Game/Entity.hpp"

Entity::Entity(BasketballCourt* map)
	:m_map(map)
{
}

Entity::~Entity()
{
}

void Entity::UpdatePhysics(float fixedDeltaSeconds)
{
	if (m_isDead)
	{
		return;
	}
	if (m_isStatic)
	{
		return;
	}

	if (m_isGravityEnabled)
	{
		AddForce(Vec3(0.f, 0.f, MODIFIED_GRAVITY_RATE) * m_mass);
	}

	AddForce(m_velocity * -m_drag);

	m_velocity += m_acceleration * fixedDeltaSeconds;
	m_position += m_velocity * fixedDeltaSeconds;

	m_acceleration = Vec3::ZERO;
	m_angularAcceleration = Vec3::ZERO;
}

void Entity::AddForce(Vec3 force)
{
	m_acceleration += force / m_mass;
}

void Entity::AddImpulse(Vec3 impulse)
{
	m_velocity += impulse / m_mass;
}

void Entity::AddTorque(Vec3 pos, Vec3 force)
{
	Vec3 toPos = pos - m_position;
	m_angularAcceleration += CrossProduct3D(toPos, force);
}

void Entity::AddTorque(Vec3 torque)
{
	m_angularAcceleration += torque;
}

void Entity::AddImpulseTorque(Vec3 torque)
{
	m_angularVelocity += torque;
}

void Entity::AddImpulseTorque(Vec3 pos, Vec3 force)
{
	Vec3 toPos = pos - m_position;
	m_angularVelocity += CrossProduct3D(toPos, force);
}

Vec2 Entity::GetPositionXY() const
{
	return Vec2(m_position.x, m_position.y);
}

FloatRange Entity::GetHeightRange() const
{
	return FloatRange(m_position.z, m_position.z + m_height);
}

Mat44 Entity::GetModeMatrix() const
{
	Mat44 modelMat = Mat44();
	modelMat = m_orientationDegrees.GetAsMatrix_IFwd_JLeft_KUp();
	modelMat.SetTranslation3D(m_position);
	return modelMat;
}
