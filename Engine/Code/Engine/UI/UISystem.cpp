#include "Engine/UI/UISystem.hpp"
#include "Engine/UI/UIElement.hpp"

UISystem::UISystem(UIConfig config)
{
	m_window = config.m_window;
	m_renderer = config.m_renderer;
	m_inputSystem = config.m_inputSystem;
	m_font = config.m_font;
}

void UISystem::Startup()
{

}

void UISystem::BeginFrame()
{

}

void UISystem::EndFrame()
{

}

void UISystem::Shutdown()
{

}

Window* UISystem::GetWindow() const
{
	return m_window;
}

Renderer* UISystem::GetRenderer() const
{
	return m_renderer;
}

InputSystem* UISystem::GetInputSystem() const
{
	return m_inputSystem;
}

BitmapFont* UISystem::GetFont() const
{
	return m_font;
}

UICallbackFunction::UICallbackFunction(UICallbackFunctionPointer callback, size_t id)
	:m_callbackPtr(callback), m_callbackID(id)
{

}

size_t UIEvent::AddListener(UICallbackFunctionPointer const& callback)
{
	for (size_t i = 0; i < m_callbackList.size(); i++)
	{
		if (!m_callbackList[i])
		{
			m_callbackList[i] = new UICallbackFunction(callback, i);
			return i;
		}
	}

	m_callbackList.push_back(new UICallbackFunction(callback, m_callbackList.size()));
	return  m_callbackList.size();
}

void UIEvent::RemoveListener(size_t callbackID)
{
	for (size_t i = 0; i < m_callbackList.size(); i++)
	{
		if (m_callbackList[i]->m_callbackID == callbackID)
		{
			m_callbackList.erase(m_callbackList.begin() + i);
			i--;
		}
	}
}

void UIEvent::Invoke()
{
	for (size_t i = 0; i < m_callbackList.size(); i++)
	{
		m_callbackList[i]->m_callbackPtr();
	}
} 

TextSetting::TextSetting(std::string text, float height, Rgba8 color, Vec2 alignment, float aspect)
	:m_text(text), m_height(height), m_color(color), m_alignment(alignment), m_aspect(aspect)
{

}