#ifndef GLOBALS_H
#define GLOBALS_H


namespace globals{
	constexpr unsigned maxMessageSize{ 4'096 };
	constexpr unsigned recvBufferSize{ 8'192 };
	constexpr char commandChar{ '/' };
}

#endif