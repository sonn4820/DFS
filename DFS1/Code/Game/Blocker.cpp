#include "Blocker.hpp"

Blocker::Blocker(BasketballCourt* map, BlockerType type, Vec3 pos, float width, float minHeight, float maxHeight, float time)
	:Entity(map), m_minHeight(minHeight), m_maxHeight(maxHeight)
{
	m_timer = new Timer(time, Clock::s_theSystemClock);
	m_radius = width;
	m_position = pos;
	m_type = type;
	m_timer->Start();
	if (type == BlockerType::STATIC_BLOCK)
	{
		m_height = maxHeight;
		m_texture = g_theGame->m_lebronBlockerTexture;
	}

	if (type == BlockerType::CONTINUOUS_BLOCK)
	{
		m_texture = g_theGame->m_lukaBlockerTexture;
	}

	if (type == BlockerType::TIMER_BLOCK)
	{
		m_texture = g_theGame->m_damianBlockerTexture;
	}
}

Blocker::~Blocker()
{
}

void Blocker::Update(float deltaSeconds)
{
	UNUSED(deltaSeconds);

	switch (m_type)
	{
	case BlockerType::STATIC_BLOCK:
		return;
		break;
	case BlockerType::CONTINUOUS_BLOCK:
		if (m_timer->DecrementPeriodIfElapsed())
		{
			m_sign *= -1;
		}

		if (m_sign > 0)
		{
			m_height = Interpolate(m_minHeight, m_maxHeight, m_timer->GetElapsedFraction());
		}
		else
		{
			m_height = Interpolate(m_maxHeight, m_minHeight, m_timer->GetElapsedFraction());
		}
		break;
	case BlockerType::TIMER_BLOCK:
		if (m_timer->DecrementPeriodIfElapsed())
		{
			m_sign *= -1;
		}
		if (m_sign > 0)
		{
			m_height = Interpolate(m_height, m_maxHeight, m_timer->GetElapsedFraction());
		}
		else
		{
			m_height = Interpolate(m_height, m_minHeight, m_timer->GetElapsedFraction());
		}
		break;
	}
}

void Blocker::Render() const
{
	std::vector<Vertex_PCU> verts;
	if (g_debugDrawing)
	{
		AddVertsForWireframeAABB3D(verts, AABB3(Vec3(0, 0, 0), m_height, 1, m_radius), Rgba8::COLOR_BLACK);
	}

	AddVertsForQuad3D(verts, Vec3(0, -m_radius, 0), Vec3(0, m_radius, 0), Vec3(0, -m_radius, m_height), Vec3(0, m_radius, m_height));

	g_theRenderer->SetBlendMode(BlendMode::ALPHA);
	g_theRenderer->SetDepthStencilMode(DepthMode::ENABLED);
	g_theRenderer->BindTexture(m_texture);
	g_theRenderer->SetModelConstants(GetModeMatrix(), m_color);
	g_theRenderer->DrawVertexArray((int)verts.size(), verts.data());
}

Vec3 Blocker::GetNearestPoint(Vec3 const point)
{
	return GetNearestPointOnAABB3D(point, AABB3(m_position, m_height, 1, m_radius));
}

void Blocker::PlaySound(SoundID sound)
{
	float vol = g_gameConfigBlackboard.GetValue("sound", 1.f);
	if (g_gameConfigBlackboard.GetValue("debugMuteAll", false))
	{
		vol = 0.f;
	}
	g_theAudio->StartSound(sound, false, vol * 2.5f);
}
