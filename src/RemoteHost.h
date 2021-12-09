#ifndef REMOTEHOST_H
#define REMOTEHOST_H

#include "includes.h"
#include "NetworkSocket.h"

class RemoteHost
{
private:
	std::string m_ip{};
	int m_port{};
	bool m_connected{};
	bool m_listening{};
	NetworkSocket m_socket{};

	void sendStreamControl() {}
	void receiveStreamControl() {}

public:
	RemoteHost(const std::string& ip = "", int port = 0);
	~RemoteHost();

	bool connect();
	void disconnect();
	bool listen();
	bool sendMessage(const std::string& message);
	std::string receiveMessage();

	// setters / mutators
	void setIp(const std::string& ip);
	void setPort(int port);

	// getters / accessors
	bool connected() const;
	bool listening() const;
	const std::string& getIp() const;
	int getPort() const;
};

#endif