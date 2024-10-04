#pragma once

#define WIN32_LEAN_AND_MEAN	
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

#include "Engine/Core/StringUtils.hpp"

enum class Mode
{
	NONE = 0,
	CLIENT,
	SERVER
};
struct NetWorkConfig
{
	std::string m_modeString;
	std::string m_hostAddressString;
	int m_sendBufferSize = 2048;
	int m_recvBufferSize = 2048;

};

class NetWorkSystem
{
	NetWorkSystem(NetWorkConfig config);
	virtual ~NetWorkSystem();

	void						Startup();
	void						Shutdown();
	virtual void				BeginFrame();
	virtual void				EndFrame();


private:
	NetWorkConfig m_config;
	SOCKET m_socket;
	Mode m_mode;
};