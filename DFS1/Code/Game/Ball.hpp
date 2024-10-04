#pragma once
#include "Game/GameCommon.hpp"
#include "Game/Entity.hpp"

class Ball: public Entity
{
public:
	Ball(BasketballCourt* map);
	virtual ~Ball();
	
	virtual void Update(float deltaSeconds) override;
	virtual void UpdatePhysics(float fixedDeltaSeconds) override;
	virtual void Render() const override;
	virtual Mat44 GetModeMatrix() const override;
	void PlaySound(SoundID sound);
public:
	bool						m_isSimulatingPhysics = true;
	VertexBuffer*				m_vbo			= nullptr;
	std::vector<Vertex_PCU>		m_vertexes;
	Quaternion					m_rotation;
	float						m_inertia;
	Rgba8						m_color = Rgba8::COLOR_WHITE;
	Texture*					m_texture = nullptr;
	bool						m_isGarbage;
	SoundPlaybackID				m_currentSound;
	float						m_timeSincePlaySound = 0.f;
};
