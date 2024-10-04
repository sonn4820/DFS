#include "Ball.hpp"

Ball::Ball(BasketballCourt* map)
	:Entity(map)
{
	EulerAngles euler = EulerAngles(0.f, 0.f, 90.f);
	m_rotation = Quaternion(euler);
	m_inertia = (2.f / 5.f) * m_mass * m_radius * m_radius;

	m_mass = BALL_MASS;
	m_radius = BALL_RADIUS;
	m_drag = 0.5f * (m_mass / (4 / 3 * PI * m_radius * m_radius)) * PI * 0.47f; // DRAG_COEFFICIENT;

	AddVertsForSphere(m_vertexes, Vec3::ZERO, m_radius);

	m_vbo = g_theRenderer->CreateVertexBuffer(sizeof(Vertex_PCU) * (int)m_vertexes.size());
	g_theRenderer->CopyCPUToGPU(m_vertexes.data(), (int)(m_vertexes.size() * sizeof(Vertex_PCU)), m_vbo);

	m_angularVelocity = Vec3::ZERO;
}

Ball::~Ball()
{
	delete m_vbo;
	m_vbo = nullptr;
}

void Ball::Update(float deltaSeconds)
{
	m_timeSincePlaySound += deltaSeconds;

	if (m_position.x < -100.f || m_position.x > 100.f)
	{
		m_isGarbage = true;
	}
	if (m_position.y < -100.f || m_position.y > 100.f)
	{
		m_isGarbage = true;
	}
	if (m_position.z < 5.f || m_position.z > 200.f)
	{
		m_isGarbage = true;
	}

	if (g_debugDrawing)
	{
		DebugAddWorldArrow(m_position, m_position + m_velocity, 0.025f, 0.f, Rgba8::COLOR_YELLOW);
		DebugAddWorldArrow(m_position, m_position + m_angularVelocity, 0.025f, 0.f, Rgba8::COLOR_VIOLET);
		DebugAddWorldArrow(m_position, m_position + GetModeMatrix().GetIBasis3D(), 0.025f, 0.f, Rgba8::COLOR_RED);
		DebugAddWorldArrow(m_position, m_position + GetModeMatrix().GetJBasis3D(), 0.025f, 0.f, Rgba8::COLOR_GREEN);
		DebugAddWorldArrow(m_position, m_position + GetModeMatrix().GetKBasis3D(), 0.025f, 0.f, Rgba8::COLOR_BLUE);
	}
}

void Ball::UpdatePhysics(float fixedDeltaSeconds)
{
	if (!m_isSimulatingPhysics)
	{
		return;
	}
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
	AddTorque(m_angularVelocity * -m_inertia);
	AddForce(0.47f * (CrossProduct3D(m_angularVelocity, m_velocity)));

	m_angularVelocity += m_angularAcceleration * fixedDeltaSeconds;

	m_rotation += m_rotation * Quaternion(m_angularVelocity) * 0.5f * fixedDeltaSeconds;
	m_rotation.Normalize();

	m_velocity += m_acceleration * fixedDeltaSeconds;
	m_position += m_velocity * fixedDeltaSeconds;

	m_acceleration = Vec3::ZERO;
	m_angularAcceleration = Vec3::ZERO;
}

void Ball::Render() const
{
	g_theRenderer->SetBlendMode(BlendMode::ALPHA);
	g_theRenderer->SetDepthStencilMode(DepthMode::ENABLED);
	g_theRenderer->BindTexture(m_texture);
	g_theRenderer->SetModelConstants(Ball::GetModeMatrix(), m_color);
	g_theRenderer->DrawVertexBuffer(m_vbo, m_vertexes.size());
}

Mat44 Ball::GetModeMatrix() const
{
	Mat44 modelMat = Mat44();
	modelMat = m_rotation.GetMatrix();
	modelMat.SetTranslation3D(m_position);
	return modelMat;
}

void Ball::PlaySound(SoundID sound)
{
	if (m_timeSincePlaySound < 0.1f)
	{
		return;
	}
	float vol = g_gameConfigBlackboard.GetValue("sound", 1.f);
	if (g_gameConfigBlackboard.GetValue("debugMuteAll", false))
	{
		vol = 0.f;
	}
	if (g_theAudio->IsPlaying(m_currentSound))
	{
		g_theAudio->StopSound(m_currentSound);
	}
	m_currentSound = g_theAudio->StartSoundAt(sound, m_position, false, vol * 4.f);
	m_timeSincePlaySound = 0.f;
}
