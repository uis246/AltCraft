#pragma once

#include <SDL2/SDL_net.h>

#include <string>
#include <vector>
#include <stdint.h>

class Socket final {
	IPaddress server;
	TCPsocket socket;

	std::vector<uint8_t> buffer;

	void Flush();
public:
	Socket(std::string &addr, uint16_t port);
	~Socket() noexcept;

	void ReadData(uint8_t *buffPtr, size_t buffLen);
	void SendData(uint8_t *buffPtr, size_t buffLen, bool more = false);

	void Connect(uint8_t *buffPtr, size_t buffLen);
};

