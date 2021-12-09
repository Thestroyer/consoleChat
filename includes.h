#ifndef INCLUDES_H
#define INCLUDES_H

#include "globals.h"

#include <iostream>
#include <string>
#include <thread>

#ifdef _WIN32
#include <WS2tcpip.h>
#include <Windows.h>

#pragma comment (lib, "ws2_32.lib")

/*
	Colliding macro from Windows.h with std::numeric_limits<>::max(),
	used for std::cin.ignore()
*/
#undef max

/*
	WS2tcpip.h declare shutdown macro for both recv
	and send as SD_BOTH while sys/socket.h declare it
	as SHUT_RDWR
*/
#define SHUT_RDWR SD_BOTH

#else
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
using SOCKET = int;

#endif


#endif