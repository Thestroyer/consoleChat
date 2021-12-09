#include "NetworkSocket.h"

bool NetworkSocket::initWinsock()
{
#ifdef _WIN32
	WORD version{ MAKEWORD(2, 2) };
	WSADATA data{};
	if (WSAStartup(version, &data) != 0) {
		std::cout << "Winsock error.\n";
		m_internalError = true;
		return false;
	}
#endif
	return true;
}

bool NetworkSocket::createSocket()
{
	m_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_socket == INVALID_SOCKET) {
		std::cout << "Can't create socket.\n";
		m_internalError = true;
		return false;
	}
	return true;
}

bool NetworkSocket::startListening(int port, std::string& returningIp)
{
	m_hint.sin_family = AF_INET;
	m_hint.sin_port = htons(port);
	SOCKET clientSocket{ socket(AF_INET, SOCK_STREAM, 0) };
	if (clientSocket == INVALID_SOCKET) {
		std::cout << "Invalid socket.\n";
		m_internalError = true;
		return false;
	}
	bind(clientSocket, reinterpret_cast<sockaddr*>(&m_hint), sizeof(m_hint));
	listen(clientSocket, SOMAXCONN);
	socklen_t clientSize = sizeof(m_hint);
	
	m_socket = accept(clientSocket, reinterpret_cast<sockaddr*>(&m_hint), &clientSize);
	if (m_socket == INVALID_SOCKET) { 
		std::cout << "Client socket error.\n";
		m_internalError = true;
		return false;
	}
	char host[NI_MAXHOST]{};
	char service[NI_MAXHOST]{};
	getnameinfo(reinterpret_cast<sockaddr*>(&m_hint), sizeof(m_hint), host, NI_MAXHOST, service, NI_MAXSERV, 0);
	inet_ntop(AF_INET, &m_hint.sin_addr, host, NI_MAXHOST);
	returningIp = host;
#ifdef _WIN32
	closesocket(clientSocket);
#endif
	shutdown(clientSocket, SHUT_RDWR);
	return true;
}

NetworkSocket::~NetworkSocket()
{
	reset();
}

bool NetworkSocket::create(Type type, int port, std::string& ip)
{
	if (!initWinsock()) return false;
	if (!createSocket()) return false;
	if (port <= 0 || port > 65535) return false;

	switch (type) {

	case Type::connect:
		if (ip.empty()) {
			m_internalError = true;
			return false;
		}
		m_hint.sin_family = AF_INET;
		m_hint.sin_port = htons(port);
		inet_pton(AF_INET, ip.c_str(), &m_hint.sin_addr);
		
		if ((connect(m_socket, reinterpret_cast<sockaddr*>(&m_hint), sizeof(m_hint))) == SOCKET_ERROR) {
			std::cout << "SOCKET ERROR.\n";
			m_internalError = true;
			return false;
		}
		return true;
	case Type::listen:
		return startListening(port, ip);
	default:
		std::cout << "Unknown connection type.\n";
		return false;
	}
}

void NetworkSocket::reset()
{
	shutdown(m_socket, SHUT_RDWR);
#ifdef _WIN32
	 closesocket(m_socket);
	 WSACleanup();
#endif
}

NetworkSocket::ErrorCodes NetworkSocket::sendText(const char* buffer, int size, int flags)
{
	if (m_internalError) return ErrorCodes::internal_error;
	long sizeSent{ send(m_socket, buffer, size, flags) };

	if (sizeSent == SOCKET_ERROR) {
		m_internalError = true;
		return ErrorCodes::socket_error;
	}
	else if (sizeSent == size) {
		return ErrorCodes::success;
	}
	else if (sizeSent != size) {
		return ErrorCodes::incomplete_delivery;
	}
	else {
		m_internalError = true;
		return ErrorCodes::unknown_error;
	}
}

NetworkSocket::ErrorCodes NetworkSocket::receiveText(std::string& buffer, int flags)
{
	if (m_internalError) return ErrorCodes::internal_error;
	char recvBuffer[globals::recvBufferSize]{};
	long sizeReceived{ recv(m_socket, recvBuffer, globals::recvBufferSize, flags) };
	while (sizeReceived == globals::recvBufferSize) {
		buffer.append(recvBuffer, sizeReceived);
		sizeReceived = recv(m_socket, recvBuffer, globals::recvBufferSize, flags);
		/*
			!BUG HERE!
			If someone will send message with size of recvBufferSize
			the next recv function (this one in while loop) will wait
			for next byte (which can be even a single '\n').

			It can be probably repaired with receiving 1 byte more
			than size of buffer, but this will make receiving
			asynchronous and it requires advanced synchronizing
			algorithm.
		*/
	}
	if (sizeReceived == SOCKET_ERROR) {
		m_internalError = true;
		return ErrorCodes::socket_error;
	}
	else if (sizeReceived == 0) {
		return ErrorCodes::client_disconnected;
	}
	buffer.append(recvBuffer, sizeReceived);

	return ErrorCodes::success;
}


bool NetworkSocket::internalError() const
{
	return m_internalError;
}
