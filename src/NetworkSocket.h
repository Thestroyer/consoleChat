#ifndef NETWORKSOCKET_H
#define NETWORKSOCKET_H

#include "includes.h"


class NetworkSocket
{
public:
	enum class Type {
		listen,
		connect,

		max_types
	};
	enum class ErrorCodes {
		socket_error,
		incomplete_delivery,
		success,
		unknown_error,
		client_disconnected,
		internal_error,

		max_error_codes
	};

private:
	SOCKET m_socket{};
	sockaddr_in m_hint{};
	Type m_type{};
	bool m_internalError{};

	bool initWinsock();
	/*
		!WARNING!
		This function is really fucked up!
		This is temporary function. It has to be redesigned.
		`type` variable has to be deleted and then logic of
		listening functionality has to be redesigned. And
		There is still a lot of bugs!
	*/
	bool createSocket();
	bool startListening(int port, std::string& returningIp);
public:
	NetworkSocket() = default;
	~NetworkSocket();

	bool create(Type type, int port, std::string& ip);
	void reset();
	ErrorCodes sendText(const char* buffer, int size, int flags);
	ErrorCodes receiveText(std::string& buffer, int flags);

	// getters / accessors
	bool internalError() const;

};

#endif