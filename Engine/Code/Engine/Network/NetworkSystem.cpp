#include "Engine/Network/NetworkSystem.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

NetWorkSystem::NetWorkSystem(NetWorkConfig config)
	:m_config(config)
{
	m_mode = Mode::NONE;
	m_socket = INVALID_SOCKET;
}

NetWorkSystem::~NetWorkSystem()
{

}

void NetWorkSystem::Startup()
{
#if defined(ENGINE_DISABLE_NETWORK)
	return;
#endif

	WSADATA wsaData;
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0)
	{
		ERROR_AND_DIE("WSAStartup failed");
}

	// Determine mode
	if (m_config.m_modeString == "CLIENT")
	{
		m_mode = Mode::CLIENT;
	}
	else if (m_config.m_modeString == "SERVER")
	{ 
		m_mode = Mode::SERVER;
	}
	else
	{
		m_mode = Mode::NONE;
	}
	
	// Create socket
	m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_socket == INVALID_SOCKET)
	{
		WSACleanup();
		ERROR_AND_DIE("Socket creation failed");
	}

	// Set buffer sizes
	setsockopt(m_socket, SOL_SOCKET, SO_SNDBUF, (char*)&m_config.m_sendBufferSize, sizeof(int));
	setsockopt(m_socket, SOL_SOCKET, SO_RCVBUF, (char*)&m_config.m_recvBufferSize, sizeof(int));

	// Setup address info
	sockaddr_in address;
	address.sin_family = AF_INET;
	inet_pton(AF_INET, m_config.m_hostAddressString.c_str(), &(address.sin_addr));
	address.sin_port = htons(12345);  

	// Connect or bind based on mode
	if (m_mode == Mode::CLIENT)
	{
		result = connect(m_socket, (sockaddr*)&address, sizeof(address));
		if (result == SOCKET_ERROR)
		{
			closesocket(m_socket);
			WSACleanup();
			ERROR_AND_DIE("Connection failed");
		}
	}
	else if (m_mode == Mode::SERVER)
	{
		result = bind(m_socket, (sockaddr*)&address, sizeof(address));
		if (result == SOCKET_ERROR)
		{
			closesocket(m_socket);
			WSACleanup();
			ERROR_AND_DIE("Bind failed");
		}

		result = listen(m_socket, SOMAXCONN);
		if (result == SOCKET_ERROR)
		{
			closesocket(m_socket);
			WSACleanup();
			ERROR_AND_DIE("Listen failed");
		}
	}
}

void NetWorkSystem::Shutdown()
{
#if defined(ENGINE_DISABLE_NETWORK)
	return;
#endif
}

void NetWorkSystem::BeginFrame()
{
#if defined(ENGINE_DISABLE_NETWORK)
	return;
#endif
}

void NetWorkSystem::EndFrame()
{
#if defined(ENGINE_DISABLE_NETWORK)
	return;
#endif
}
