#pragma once
#include "Engine/UI/UIElement.hpp"
#include "Engine/UI/Canvas.hpp"

class Panel : public UIElement
{
public:
	Panel(Canvas* canvas, UIElement* parent = nullptr);
	Panel(Canvas* canvas, AABB2 bound, UIElement* parent = nullptr);
	Panel(Canvas* canvas, AABB2 bound, Rgba8 color, UIElement* parent = nullptr);
	Panel(Canvas* canvas, AABB2 bound, Rgba8 color, Texture* image, UIElement* parent = nullptr);
	virtual ~Panel();

	void Startup() override;
	void Update(float deltaSeconds) override;
	void Render() const override;
	void Shutdown() override;

	void SetCanvas(Canvas* canvas) override;
	void SetBound(AABB2 bound);
	void SetTexture(Texture* image);
	void SetColor(Rgba8 color);

	bool IsMouseInBound() const;
protected:
	Canvas* m_canvas = nullptr;
	AABB2 m_bound;
	Texture* m_texture;
	Rgba8 m_color = Rgba8(200, 200, 200, 150);
	bool m_isMouseInBound = false;

	std::vector<Vertex_PCU> m_verts;
	VertexBuffer* m_buffer = nullptr;
};