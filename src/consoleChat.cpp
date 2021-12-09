#include "NetworkSocket.h"
#include "RemoteHost.h"


namespace globals {
	bool muted{ false };
	const char* hostColor{ "\x1B[38;5;34m" }; // green
	const char* remoteHostColor{ "\x1B[38;5;33m" }; // blue
}

std::string getCurrentTime() {
	time_t now = std::time(nullptr);
	std::string tmp{};
	tm time{};
#ifdef _WIN32
	localtime_s(&time, &now);
#else	
	time = *std::localtime(&now);
#endif
	if (time.tm_hour < 10) {
		tmp += '0';
	}
	tmp += std::to_string(time.tm_hour);
	tmp += ':';
	if (time.tm_min < 10) {
		tmp += '0';
	}
	tmp += std::to_string(time.tm_min);
	tmp += ':';
	if (time.tm_sec < 10) {
		tmp += '0';
	}
	tmp += std::to_string(time.tm_sec);

	return tmp;
}

void receiver(RemoteHost& rHost) {
	std::string message{};
	while (rHost.connected()) {
		message = rHost.receiveMessage();
		if (message.empty()) break;
#ifdef _WIN32
		if (!globals::muted) {
			FlashWindow(GetConsoleWindow(), true);
		}
#endif
		std::cout << "\x1B[2K"; // Clear entire line
		std::cout << "\x1B[G"; // Set absolute cursor position to default (column 1)
		std::cout << globals::remoteHostColor; // Set blue font color
		std::cout << "[" << getCurrentTime() << "]" << rHost.getIp() << ": " << message;
		std::cout << "\x1B[38;5;15m" << '\n'; // Set white font color
		std::cout << '>' << std::flush;
	}
}

void sender(RemoteHost& rHost) {
	std::string message{};
	while (rHost.connected()) {
		std::cout << '>';
		std::getline(std::cin, message);
		std::cout << "\x1B[A";
		std::cout << "\x1B[2K";
		std::cout << "\x1B[G";
		std::cout << globals::hostColor;
		std::cout << "[" << getCurrentTime() << "]> " << message << '\n';
		std::cout << "\x1B[38;5;15m";
		
		if (message == globals::commandChar + static_cast<std::string>("exit")) {
			rHost.disconnect();
			std::cout << "Disconnected.\n";
			break;
		}
		else if (message == globals::commandChar + static_cast<std::string>("mute")) {
			if (globals::muted) {
				std::cout << "Conversation unmuted.\n";
				globals::muted = false;
			}
			else {
				std::cout << "Conversation muted.\n";
				globals::muted = true;
			}
		}
		else if (message == globals::commandChar + static_cast<std::string>("help")) {
			std::cout << "Commands are given after '" << globals::commandChar << "' character.\n";
			std::cout << "Current commands: \n";
			std::cout << globals::commandChar << "help - prints this help page.\n";
			std::cout << globals::commandChar << "mute - mutes this conversation (minimalized application doesn't blink, when new message is received).\n";
			std::cout << globals::commandChar << "exit - closes this conversation.\n";
		}
		else if (!rHost.sendMessage(message)) {
			std::cout << "Message has not been sent :(\n";
		}
	}
}

bool listener(RemoteHost& rHost) {
	std::cout << "Starting listening on port " << rHost.getPort() << '\n';
	if (rHost.listen()) {
		std::cout << "\x1B[2J"; // Clear entire screen
		std::cout << "\x1B[1;1H"; // Move cursor to 1,1
		std::cout << rHost.getIp() << ':' << rHost.getPort() << " connected.\n";
		return true;
	}
	return false;
}

#ifdef _WIN32
bool setupWindowsConsole() {

	HANDLE hOut{ GetStdHandle(STD_OUTPUT_HANDLE) };
	DWORD dwMode{};
	if (hOut == INVALID_HANDLE_VALUE)
		return false;

	if (!GetConsoleMode(hOut, &dwMode))
		return false;
	
	dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;

	if (!SetConsoleMode(hOut, dwMode))
		return false;

	return true;
}
#endif

void openChat(RemoteHost& rHost) {
	std::cin.clear();
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	std::cout << "Enter " << globals::commandChar << "help for command list.\n";

	std::thread receiving{ receiver, std::ref(rHost) };
	sender(rHost);
	receiving.detach();
}

int main() {
#ifdef _WIN32
	if (!setupWindowsConsole()) {
		std::cerr << "Failed to setup windows console.\n";
		return GetLastError();
	}
#endif

	std::string hostIpPort{};
	do {
		std::cout << "[IP:port]: ";
		std::cin >> hostIpPort;

		if (hostIpPort == "help") {
			std::cout << "You have to enter someone's IP address, then colon ':', and then port number (1-65535).\n";
			std::cout << "If you want to someone connect to you, then you have to enter listening port only, then someone can connect to this port."
				" WARNING! You have to disable your PC's firewall additionally, or simply allow this program to accept incoming connections.\n";
		}
	} while (hostIpPort == "help");

	std::string ip{};
	int port{};
	std::size_t foundColon{ hostIpPort.find(':') };
	if (foundColon != std::string::npos) {
		ip = hostIpPort.substr(0, foundColon);
	}
	try {
		if (foundColon == std::string::npos) {
			port = std::stoi(hostIpPort);
		}
		else {
			port = std::stoi(hostIpPort.substr(foundColon + 1, hostIpPort.size()));
		}
		
	}
	catch (...) {
		std::cout << "Enter valid port after ':' (1-65535)!\n";
		return 1;
	}


	RemoteHost rHost{};
	rHost.setPort(port);

	if (ip == "localhost" || ip == "127.0.0.1" || ip.empty()) {
		if (listener(rHost)) {
			openChat(rHost);
		}
		else {
			return 2;
		}
	}
	else {
		rHost.setIp(ip);
		std::cout << "Connecting..\n";
		if (rHost.connect()) {
			std::cout << "\x1B[2J"; // Clear entire screen
			std::cout << "\x1B[H"; // Move cursor to 1,1
			std::cout << "Connected to " << rHost.getIp() << ':' << rHost.getPort() << '\n';
			openChat(rHost);
		}
		else {
			return 3;
		}
	}
	rHost.disconnect();
	std::cin.get();

	return 0;
}