#pragma once
#include "Engine/UI/UIElement.hpp"
#include "Engine/UI/Canvas.hpp"

struct Button : public UIElement
{
	Button(Canvas* canvas, UIElement* parent = nullptr);
	Button(Canvas* canvas, AABB2 bound, UIElement* parent = nullptr);
	Button(Canvas* canvas, AABB2 bound, TextSetting textSetting, UIElement* parent = nullptr);
	Button(Canvas* canvas, AABB2 bound, TextSetting textSetting, Rgba8 hoverColor, Rgba8 pressedColor, Rgba8 unhoverColor, UIElement* parent = nullptr);
	Button(Canvas* canvas, AABB2 bound, TextSetting textSetting, Texture* hover, Texture* pressed, Texture* unhover, UIElement* parent = nullptr);
	Button(Canvas* canvas, AABB2 bound, TextSetting textSetting, Texture* hover, Texture* pressed, Texture* unhover, Rgba8 hoverColor, Rgba8 pressedColor, Rgba8 unhoverColor, UIElement* parent = nullptr);
	virtual ~Button();

	void Startup() override;
	void Update(float deltaSeconds) override;
	void Render() const override;
	void Shutdown() override;

	void SetCanvas(Canvas* canvas) override;
	void SetBound(AABB2 bound);
	void SetText(TextSetting textSetting);

	void SetTextureHover(Texture* image);
	void SetTexturePressed(Texture* image);
	void SetTextureUnhover(Texture* image);

	void SetColorHover(Rgba8 color);
	void SetColorPressed(Rgba8 color);
	void SetColorUnhover(Rgba8 color);

	void OnClickEvent(UICallbackFunctionPointer const& callback);
	void OnHoverEvent(UICallbackFunctionPointer const& callback);
	void OnUnhoverEvent(UICallbackFunctionPointer const& callback);
	void OnPressedEvent(UICallbackFunctionPointer const& callback);

protected:
	Canvas* m_canvas = nullptr;

	UIEvent m_onClickEvent;
	UIEvent m_onHoverEvent;
	UIEvent m_onUnhoverEvent;
	UIEvent m_onPressedEvent;

	std::vector<Vertex_PCU> m_textVerts;
	VertexBuffer* m_textBuffer = nullptr;

	std::vector<Vertex_PCU> m_buttonVerts;
	VertexBuffer* m_buttonBuffer = nullptr;

	AABB2 m_bound;
	TextSetting m_textSetting;

	Texture* m_textureHover = nullptr;
	Texture* m_texturePressed = nullptr;
	Texture* m_textureUnhover = nullptr;

	Rgba8 m_colorUnhover = Rgba8(200, 200, 200);
	Rgba8 m_colorHover = Rgba8(255, 255, 255);
	Rgba8 m_colorPressed = Rgba8(150, 150, 150);

private:
	Texture* m_currentTexture = nullptr;
	Rgba8 m_currentColor;
};