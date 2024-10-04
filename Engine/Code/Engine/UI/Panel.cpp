#include "Engine/UI/Panel.hpp"

Panel::Panel(Canvas* canvas, UIElement* parent /*= nullptr*/)
	:Panel(canvas, AABB2(), Rgba8::COLOR_WHITE, nullptr, parent)
{

}

Panel::Panel(Canvas* canvas, AABB2 bound, UIElement* parent /*= nullptr*/)
	:Panel(canvas, bound, Rgba8::COLOR_WHITE, nullptr, parent)
{

}

Panel::Panel(Canvas* canvas, AABB2 bound, Rgba8 color, UIElement* parent /*= nullptr*/)
	:Panel(canvas, bound, color, nullptr, parent)
{

}

Panel::Panel(Canvas* canvas, AABB2 bound, Rgba8 color, Texture* image, UIElement* parent /*= nullptr*/)
{
	SetCanvas(canvas);
	SetBound(bound);
	SetColor(color);
	SetTexture(image);
	if (parent)
	{
		m_parent->AddChild(this);
	}
	else
	{
		canvas->AddChild(this);
	}

	Startup();
}

Panel::~Panel()
{
	Shutdown();
}

void Panel::Startup()
{
	if (m_parent)
	{
		SetActive(m_parent->IsActive());
	}
}

void Panel::Update(float deltaSeconds)
{
	if (!m_isActive || !m_interactive)
	{
		return;
	}

	m_delayTimer -= deltaSeconds;
	if (m_delayTimer > 0.f)
	{
		return;
	}

	Vec2 mouseNormalizedPos = m_canvas->GetSystemInputSystem()->GetCursorNormalizedPosition();
	Vec2 mousePos = mouseNormalizedPos * m_canvas->GetCamera()->GetOrthographicTopRight();
	m_isMouseInBound = m_bound.IsPointInside(mousePos);

	for (size_t i = 0; i < m_children.size(); i++)
	{
		m_children[i]->Update(deltaSeconds);
	}
}

void Panel::Render() const
{
	if (!m_isActive)
	{
		return;
	}

	m_canvas->GetSystemRenderer()->BeginCamera(*m_canvas->GetCamera());

	m_canvas->GetSystemRenderer()->SetRasterizerMode(RasterizerMode::SOLID_CULL_NONE);
	m_canvas->GetSystemRenderer()->SetBlendMode(BlendMode::ALPHA);
	m_canvas->GetSystemRenderer()->SetModelConstants(Mat44(), m_color);

	m_canvas->GetSystemRenderer()->BindTexture(m_texture);
	m_canvas->GetSystemRenderer()->DrawVertexBuffer(m_buffer, m_verts.size());

	for (size_t i = 0; i < m_children.size(); i++)
	{
		m_children[i]->Render();
	}

	m_canvas->GetSystemRenderer()->EndCamera(*m_canvas->GetCamera());
}

void Panel::Shutdown()
{
	delete m_buffer;
	m_buffer = nullptr;

	for (size_t i = 0; i < m_children.size(); i++)
	{
		if (m_children[i])
		{
			m_children[i]->Shutdown();
			delete m_children[i];
		}

		m_children[i] = nullptr;
	}

	m_children.clear();
}

void Panel::SetCanvas(Canvas* canvas)
{
	m_canvas = canvas;
	m_canvas->AddElementToCanvas(this);
}

void Panel::SetBound(AABB2 bound)
{
	m_bound = bound;
	AddVertsForAABB2D(m_verts, bound);
	m_buffer = m_canvas->GetSystemRenderer()->CreateVertexBuffer(sizeof(Vertex_PCU) * (unsigned int)m_verts.size());
	m_canvas->GetSystemRenderer()->CopyCPUToGPU(m_verts.data(), (int)m_verts.size() * sizeof(Vertex_PCU), m_buffer);
}

void Panel::SetTexture(Texture* image)
{
	m_texture = image;
}

void Panel::SetColor(Rgba8 color)
{
	m_color = color;
}

bool Panel::IsMouseInBound() const
{
	return m_isMouseInBound;
}
