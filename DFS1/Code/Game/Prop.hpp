#pragma once
#include "Game/GameCommon.hpp"
#include "Game/Entity.hpp"

class Prop: public Entity
{
public:
	Prop(BasketballCourt* map);
	virtual ~Prop();
	
	virtual void Update(float deltaSeconds) override;
	virtual void Render() const override;
public:
	std::vector<Vertex_PCU>		m_vertexes;
	Rgba8						m_color = Rgba8::COLOR_WHITE;
	Texture*					m_texture = nullptr;
};