#include "Prop.hpp"

Prop::Prop(BasketballCourt* map)
	:Entity(map)
{

}

Prop::~Prop()
{
}

void Prop::Update(float deltaSeconds)
{
	UNUSED(deltaSeconds);
}

void Prop::Render() const
{
	g_theRenderer->SetBlendMode(BlendMode::ALPHA);
	g_theRenderer->SetDepthStencilMode(DepthMode::ENABLED);
	g_theRenderer->BindTexture(m_texture);
	g_theRenderer->SetModelConstants(GetModeMatrix(), m_color);
	g_theRenderer->DrawVertexArray((int)m_vertexes.size(), m_vertexes.data());
}
