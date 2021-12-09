#include "RemoteHost.h"

RemoteHost::RemoteHost(const std::string& ip, int port)
	: m_ip{ ip }, m_port{ port }
{
}

RemoteHost::~RemoteHost()
{
}

bool RemoteHost::connect()
{
	if (m_ip.empty()) return false;
	if (!m_port) return false;
	if (!m_socket.create(NetworkSocket::Type::connect, m_port, m_ip)) {
		return false;
	}
	m_connected = true;
	return true;
}

void RemoteHost::disconnect()
{
	if (m_connected) {
		m_connected = false;
		m_socket.reset();
	}
}

bool RemoteHost::listen()
{
	m_listening = true;
	if (!m_socket.create(NetworkSocket::Type::listen, m_port, m_ip)) {
		m_listening = false;
		return false;
	}
	m_listening = false;
	m_connected = true;
	return true;
}

bool RemoteHost::sendMessage(const std::string& message)
{
	if (m_connected) {
		if (message.size() > globals::maxMessageSize) {
			std::cout << "Message cannot be longer than " << globals::maxMessageSize << " characters.\n";
			return false;
		}
		switch (m_socket.sendText(message.c_str(), message.size(), 0)) {
		case NetworkSocket::ErrorCodes::socket_error:
			std::cout << "Socket error.\n";
			m_connected = false;
			return false;
		case NetworkSocket::ErrorCodes::incomplete_delivery:
			std::cout << "Incomplete delivery.\n";
			return false;
		case NetworkSocket::ErrorCodes::unknown_error:
			std::cout << "Unknown error.\n";
			return false;
		case NetworkSocket::ErrorCodes::success:
			return true;
		case NetworkSocket::ErrorCodes::internal_error:
			std::cout << "Internal error.\n";
			m_connected = false;
			return false;
		default:
			// Should never happen!
			std::cout << "Something is really fucked with code.\n";
			return false;
		}
		return false;
	}
	else {
		std::cout << "Connection is not estabilished.\n";
	}
	return false;
}

std::string RemoteHost::receiveMessage()
{
	if (m_connected) {
		std::string buffer{};
		switch (m_socket.receiveText(buffer, 0)) {
		case NetworkSocket::ErrorCodes::socket_error:
			/*
				It has to be redesigned...
			*/
			if (m_connected) {
				std::cout << "Socket error.\n";
				m_connected = false;
			}
			return "";
		case NetworkSocket::ErrorCodes::client_disconnected:
			std::cout << "Client disconnected.\n";
			m_connected = false;
			return "";
		case NetworkSocket::ErrorCodes::success:
			return buffer;
		case NetworkSocket::ErrorCodes::internal_error:
			std::cout << "Internal error.\n";
			m_connected = false;
			return "";
		default:
			// Should never happen!
			std::cout << "Something is really fucked with code.\n";
			return "";
		}
	}
	return "";
}

void RemoteHost::setIp(const std::string& ip)
{
	m_ip = ip;
}

void RemoteHost::setPort(int port)
{
	m_port = port;
}

bool RemoteHost::connected() const
{
	return m_connected;
}

bool RemoteHost::listening() const
{
	return m_listening;
}

const std::string& RemoteHost::getIp() const
{
	return m_ip;
}

int RemoteHost::getPort() const
{
	return m_port;
}
