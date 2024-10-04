#include "Engine/Window/Window.hpp"
#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include <windows.h>			// #include this (massive, platform-specific) header in VERY few places (and .CPPs only)
#include <math.h>
#include <cassert>
#include <crtdbg.h>
#include <commdlg.h>
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/EventSystem.hpp"

#include "ThirdParty/imgui/imgui.h"
#include "ThirdParty/imgui/backends/imgui_impl_win32.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

Window* Window::s_theWindow = nullptr;

LRESULT CALLBACK WindowsMessageHandlingProcedure(HWND windowHandle, UINT wmMessageCode, WPARAM wParam, LPARAM lParam)
{
	Window* window = Window::GetMainWindowInstance();
	GUARANTEE_OR_DIE(window != nullptr, "Window was null");

	InputSystem* input = window->GetConfig().m_inputSystem;
	GUARANTEE_OR_DIE(input != nullptr, "Window's InputSystem pointer was null");

	if (ImGui_ImplWin32_WndProcHandler(windowHandle, wmMessageCode, wParam, lParam))
		return true;

	switch (wmMessageCode)
	{
		case WM_CHAR:
		{
			EventArgs args;
			args.SetValue("KeyCode", Stringf("%d", (unsigned char)wParam));
			FireEvent("CharInput", args);
			return 0;
		}
		// App close requested via "X" button, or right-click "Close Window" on task bar, or "Close" from system menu, or Alt-F4
		case WM_CLOSE:
		{
			FireEvent("quit");
			return 0; // "Consumes" this message (tells Windows "okay, we handled it")
		}

		// Raw physical keyboard "key-was-just-depressed" event (case-insensitive, not translated)
		case WM_KEYDOWN:
		{
			EventArgs args;
			args.SetValue("KeyCode", Stringf("%d", (unsigned char)wParam));
			FireEvent("KeyPressed", args);
			return 0;
		}

		// Raw physical keyboard "key-was-just-released" event (case-insensitive, not translated)
		case WM_KEYUP:
		{
			EventArgs args;
			args.SetValue("KeyCode", Stringf("%d", (unsigned char)wParam));
			FireEvent("KeyReleased", args);
			return 0;
		}

		case WM_LBUTTONDOWN:
		{
			unsigned char keycode = KEYCODE_LEFT_MOUSE;
			bool wasConsumed = false;
			if (input)
			{
				wasConsumed = input->HandleKeyPressed(keycode);
				if (wasConsumed)
				{
					return 0;
				}
			}
			break;
		}

		case WM_LBUTTONUP:
		{
			unsigned char keycode = KEYCODE_LEFT_MOUSE;
			bool wasConsumed = false;
			if (input)
			{
				wasConsumed = input->HandleKeyReleased(keycode);
				if (wasConsumed)
				{
					return 0;
				}
			}
			break;
		}

		case WM_RBUTTONDOWN:
		{
			unsigned char keycode = KEYCODE_RIGHT_MOUSE;
			bool wasConsumed = false;
			if (input)
			{
				wasConsumed = input->HandleKeyPressed(keycode);
				if (wasConsumed)
				{
					return 0;
				}
			}
			break;
		}
		case WM_RBUTTONUP:
		{
			unsigned char keycode = KEYCODE_RIGHT_MOUSE;
			bool wasConsumed = false;
			if (input)
			{
				wasConsumed = input->HandleKeyReleased(keycode);
				if (wasConsumed)
				{
					return 0;
				}
			}
			break;
		}
		case WM_MOUSEWHEEL:
		{
			if (IsMousePresent())
			{
				EventArgs args;
				args.SetValue("MouseWheel", Stringf("%d", GET_WHEEL_DELTA_WPARAM(wParam)));
				FireEvent("MouseScroll", args);
				return 0;
			}
			break;
		}
	}

	// Send back to Windows any unhandled/unconsumed messages we want other apps to see (e.g. play/pause in music apps, etc.)
	return DefWindowProc(windowHandle, wmMessageCode, wParam, lParam);
}

bool IsMousePresent()
{
	return GetSystemMetrics(SM_MOUSEPRESENT) != 0;
}

Window::Window(WindowConfig const& config)
	:m_config(config)
{
	s_theWindow = this;
}

void Window::Startup()
{
	CreateOSWindow();
}
void Window::BeginFrame()
{
	RunMessagePump();
}
void Window::EndFrame()
{
	//SwapBuffers((reinterpret_cast<HDC>(m_dc)));
}
void Window::Shutdown()
{
	
}

WindowConfig const& Window::GetConfig() const
{
	return m_config;
}

float Window::GetAspect() const
{
	return m_config.m_clientAspect;
}

void* Window::GetDisplayContext() const
{
	return m_dc;
}

// Vec2 Window::GetNormalizedCursorPos() const
// {
// 	HWND windowHandle = HWND(m_hwnd);
// 	POINT cursorCoords;
// 	RECT clientRect;
// 	::GetCursorPos(&cursorCoords);
// 	::ScreenToClient(windowHandle, &cursorCoords);
// 	::GetClientRect(windowHandle, &clientRect);
// 	float cursorX = float(cursorCoords.x) / float(clientRect.right);
// 	float cursorY = float(cursorCoords.y) / float(clientRect.bottom);
// 	return Vec2(cursorX, 1.f - cursorY);
// }

void* Window::GetHwnd() const
{
	return m_hwnd;
}

IntVec2 Window::GetClientDimensions() const
{
	return m_clientDimension;
}



bool Window::GetFileName(std::string& outPath)
{
	char filename[MAX_PATH]; filename[0] = '\0';

	OPENFILENAMEA data = { };
	data.lStructSize = sizeof(data);
	data.lpstrFile = filename;
	data.nMaxFile = sizeof(filename);
	data.lpstrFilter = "All\0*.*\0";
	data.nFilterIndex = 1;
	data.lpstrInitialDir = NULL;
	data.hwndOwner = (HWND)Window::GetMainWindowInstance()->GetHwnd();
	data.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	if (GetOpenFileNameA(&data))
	{
		outPath = filename;
		return true;
	}
	else
	{
		DWORD error = ::GetLastError();
		if (error != 0)
		{
			LPSTR messageBuffer = nullptr;
			DWORD flags = FORMAT_MESSAGE_ALLOCATE_BUFFER |
				FORMAT_MESSAGE_FROM_SYSTEM |
				FORMAT_MESSAGE_IGNORE_INSERTS;
			FormatMessageA(flags, NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				(LPSTR)&messageBuffer, 0, NULL);
			ERROR_RECOVERABLE(messageBuffer);
			LocalFree(messageBuffer);
		}
	}

	return false;
}

Window* Window::GetMainWindowInstance()
{
	return s_theWindow;
}

void Window::CreateOSWindow()
{
	// Define a window style/class
	WNDCLASSEX windowClassDescription;
	memset(&windowClassDescription, 0, sizeof(windowClassDescription));
	windowClassDescription.cbSize = sizeof(windowClassDescription);
	windowClassDescription.style = CS_OWNDC; // Redraw on move, request own Display Context
	windowClassDescription.lpfnWndProc = static_cast<WNDPROC>(WindowsMessageHandlingProcedure); // Register our Windows message-handling function
	windowClassDescription.hInstance = GetModuleHandle(NULL);
	windowClassDescription.hIcon = NULL;
	windowClassDescription.hCursor = NULL;
	windowClassDescription.lpszClassName = TEXT("Simple Window Class");
	RegisterClassEx(&windowClassDescription);

	// #SD1ToDo: Add support for full screen mode (requires different window style flags than windowed mode)
	const DWORD windowStyleFlags = WS_CAPTION | WS_BORDER | WS_THICKFRAME | WS_SYSMENU | WS_OVERLAPPED;
	const DWORD windowStyleExFlags = WS_EX_APPWINDOW;

	// Get desktop rect, dimensions, aspect
	RECT desktopRect;
	HWND desktopWindowHandle = GetDesktopWindow();
	GetClientRect(desktopWindowHandle, &desktopRect);
	float desktopWidth = (float)(desktopRect.right - desktopRect.left);
	float desktopHeight = (float)(desktopRect.bottom - desktopRect.top);
	float desktopAspect = desktopWidth / desktopHeight;

	// Calculate maximum client size (as some % of desktop size)
	constexpr float maxClientFractionOfDesktop = 0.9f;
	float clientWidth = desktopWidth * maxClientFractionOfDesktop;
	float clientHeight = desktopHeight * maxClientFractionOfDesktop;
	if (m_config.m_clientAspect > desktopAspect)
	{
		// Client window has a wider aspect than desktop; shrink client height to match its width
		clientHeight = clientWidth / m_config.m_clientAspect;
	}
	else
	{
		// Client window has a taller aspect than desktop; shrink client width to match its height
		clientWidth = clientHeight * m_config.m_clientAspect;
	}
	m_clientDimension = IntVec2(static_cast<int>(clientWidth), static_cast<int>(clientHeight));

	// Calculate client rect bounds by centering the client area
	float clientMarginX = 0.5f * (desktopWidth - clientWidth);
	float clientMarginY = 0.5f * (desktopHeight - clientHeight);
	RECT clientRect;
	clientRect.left = (int)clientMarginX;
	clientRect.right = clientRect.left + (int)clientWidth;
	clientRect.top = (int)clientMarginY;
	clientRect.bottom = clientRect.top + (int)clientHeight;

	// Calculate the outer dimensions of the physical window, including frame et.
	RECT windowRect = clientRect;
	AdjustWindowRectEx(&windowRect, windowStyleFlags, FALSE, windowStyleExFlags);

	WCHAR windowTitle[1024];
	MultiByteToWideChar(GetACP(), 0, m_config.m_windowTitle.c_str(), -1, windowTitle, sizeof(windowTitle) / sizeof(windowTitle[0]));
	m_hwnd = CreateWindowEx(
		windowStyleExFlags,
		windowClassDescription.lpszClassName,
		windowTitle,
		windowStyleFlags,
		windowRect.left,
		windowRect.top,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		NULL,
		NULL,
		NULL, //(HINSTANCE)applicationInstanceHandle,
		NULL);
	
	ShowWindow((HWND)m_hwnd, SW_SHOW);
	SetForegroundWindow((HWND)m_hwnd);
	SetFocus((HWND)m_hwnd);

	m_dc = GetDC((HWND)m_hwnd);

	HCURSOR cursor = LoadCursor(NULL, IDC_ARROW);
	SetCursor(cursor);
}

//-----------------------------------------------------------------------------------------------
// Processes all Windows messages (WM_xxx) for this app that have queued up since last frame.
// For each message in the queue, our WindowsMessageHandlingProcedure (or "WinProc") function
//	is called, telling us what happened (key up/down, minimized/restored, gained/lost focus, etc.)
void Window::RunMessagePump()
{
	MSG queuedMessage;
	for (;; )
	{
		const BOOL wasMessagePresent = PeekMessage(&queuedMessage, NULL, 0, 0, PM_REMOVE);
		if (!wasMessagePresent)
		{
			break;
		}
		TranslateMessage(&queuedMessage);
		DispatchMessage(&queuedMessage); // This tells Windows to call our "WindowsMessageHandlingProcedure" (a.k.a. "WinProc") function
	}
}


