#include "Engine/UI/Text.hpp"

Text::Text(Canvas* canvas, UIElement* parent /*= nullptr*/)
	:Text(canvas, Vec2(), TextSetting(), parent)
{

}

Text::Text(Canvas* canvas, Vec2 position, TextSetting text, UIElement* parent /*= nullptr*/)
{
	SetCanvas(canvas);
	SetPosition(position);
	SetTextSetting(text);

	if (parent)
	{
		parent->AddChild(this);
	}
	else
	{
		canvas->AddChild(this);
	}
	Startup();
}

Text::~Text()
{
	Shutdown();
}

void Text::Startup()
{
	if (m_parent)
	{
		SetActive(m_parent->IsActive());
	}
}

void Text::Update(float deltaSeconds)
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
}

void Text::Render() const
{
	if (!m_isActive)
	{
		return;
	}

	m_canvas->GetSystemRenderer()->BeginCamera(*m_canvas->GetCamera());

	m_canvas->GetSystemRenderer()->SetRasterizerMode(RasterizerMode::SOLID_CULL_NONE);
	m_canvas->GetSystemRenderer()->SetBlendMode(BlendMode::ALPHA);
	m_canvas->GetSystemRenderer()->SetModelConstants(Mat44(), m_textSetting.m_color);

	m_canvas->GetSystemRenderer()->BindTexture(&m_canvas->GetSystemFont()->GetTexture());
	m_canvas->GetSystemRenderer()->DrawVertexBuffer(m_textBuffer, m_textVerts.size());

	for (size_t i = 0; i < m_children.size(); i++)
	{
		m_children[i]->Render();
	}

	m_canvas->GetSystemRenderer()->EndCamera(*m_canvas->GetCamera());
}

void Text::Shutdown()
{
	delete m_textBuffer;
	m_textBuffer = nullptr;

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

void Text::SetCanvas(Canvas* canvas)
{
	m_canvas = canvas;
	m_canvas->AddElementToCanvas(this);
}

void Text::SetPosition(Vec2 pos)
{
	m_position = pos;
}

void Text::SetTextSetting(TextSetting textSetting)
{
	m_textSetting = textSetting;
	Vec2 pos = m_position - Vec2(m_textSetting.m_height, m_textSetting.m_height * m_textSetting.m_aspect) * 0.5f + Vec2(m_textSetting.m_height, m_textSetting.m_height * m_textSetting.m_aspect) * m_textSetting.m_alignment;
	m_canvas->GetSystemFont()->AddVertsForText2D(m_textVerts, pos, m_textSetting.m_height, m_textSetting.m_text, m_textSetting.m_color, m_textSetting.m_aspect);
	m_textBuffer = m_canvas->GetSystemRenderer()->CreateVertexBuffer(sizeof(Vertex_PCU) * (int)m_textVerts.size());
	m_canvas->GetSystemRenderer()->CopyCPUToGPU(m_textVerts.data(), (int)m_textVerts.size() * sizeof(Vertex_PCU), m_textBuffer);
}
