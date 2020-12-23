#include "Socket.hpp"

#include <easylogging++.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
//#include <netinet/tcp.h>
#include <linux/tcp.h>

//Options from setings
unsigned int timeout=0;//msecs

// Options from advanced settings
int tfo=0;//Since Linux 4.11
int qack=1;
int nonagle=1;
int thin=1;

const static int zero=0, ka_timeout=40;


Socket::Socket(std::string &address, uint16_t port) {
	//Resolve server address
	int result = getaddrinfo(address.c_str(), NULL, NULL, &ai);
	if (result)
		throw std::runtime_error("Hostname not resolved: " + std::to_string(result));

	//Set server port
	if (ai->ai_family == AF_INET)
		reinterpret_cast<sockaddr_in*>(ai->ai_addr)->sin_port=htons(port);
	else if (ai->ai_family == AF_INET6)
		reinterpret_cast<sockaddr_in6*>(ai->ai_addr)->sin6_port=htons(port);
	else {
		freeaddrinfo(ai);
		throw std::runtime_error("Unknown sockaddr family");
	}

	//Create socket
	sock = socket(ai->ai_family, SOCK_STREAM, IPPROTO_TCP);
	if (sock == -1) {
		freeaddrinfo(ai);
		throw std::runtime_error("Failed to create socket");
	}

	//Configure socket
#	ifdef TCP_FASTOPEN_CONNECT
	setsockopt(sock, IPPROTO_TCP, TCP_FASTOPEN_CONNECT, &tfo/*TCP Fast Open enabled in network settings*/, sizeof(tfo));
#	endif
	setsockopt(sock, IPPROTO_TCP, TCP_QUICKACK, &qack/*TCP Quick ACK enabled in network settings*/, sizeof(qack));
	setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, &nonagle/*Nagle disabled in network settings*/, sizeof(nonagle));
	setsockopt(sock, IPPROTO_TCP, TCP_THIN_DUPACK, &thin/*Thin stream enabled in network settings*/, sizeof(thin));
	setsockopt(sock, IPPROTO_TCP, TCP_THIN_LINEAR_TIMEOUTS, &thin/*Thin stream enabled in network settings*/, sizeof(thin));
	setsockopt(sock, IPPROTO_TCP, TCP_USER_TIMEOUT, &timeout, sizeof(timeout));
	setsockopt(sock, IPPROTO_TCP, TCP_KEEPCNT, &zero, sizeof(zero));
	setsockopt(sock, IPPROTO_TCP, TCP_KEEPIDLE, &ka_timeout, sizeof(ka_timeout));
}

void Socket::Connect(uint8_t *buffPtr, size_t buffLen) {
	int result;

	result = sendto(sock, buffPtr, buffLen, MSG_FASTOPEN | MSG_NOSIGNAL, ai->ai_addr, ai->ai_addrlen);

	if (result == -1) {
		if (errno == EPIPE) {
			result = connect(sock, ai->ai_addr, ai->ai_addrlen);
			if (result != -1){
				SendData(buffPtr, buffLen);
				return;
			}
		}
		throw std::runtime_error("Connection failed: " + std::string(std::strerror(errno)));
	}
}

Socket::~Socket() noexcept {
	freeaddrinfo(ai);

	for (unsigned int i = 0; i < 3; i++) {
		int ret = close(sock);
		if(ret == -1) {//Some shit happen
			if(errno == EBADF)//No such fd
				break;//Just skip closing
			else
				continue;//Retry. AFAIK if socket closed close() will return EBABF
		} else
			break;//Everything ok
	}
}

void Socket::ReadData(uint8_t *buffPtr, size_t buffLen) {
	int result;
	size_t totalReceived = 0;
	while (totalReceived < buffLen) {
		result = recv(sock, buffPtr + totalReceived, buffLen - totalReceived, MSG_WAITALL | MSG_NOSIGNAL);
		if (result == -1) {
			if(errno == EINTR)
				continue;
			else
				throw std::runtime_error("Data receiving failed: " + std::string(std::strerror(errno)));
		} else if (result == 0)
			throw std::runtime_error("Connection closed by server");
		totalReceived += result;
	}
}

void Socket::SendData(uint8_t *buffPtr, size_t buffLen, bool more) {
	int result;
	result = send(sock, buffPtr, buffLen, MSG_DONTWAIT | MSG_NOSIGNAL | (more ? MSG_MORE : 0));
	if (result == -1) {
		if (errno == EAGAIN || errno == EWOULDBLOCK) {
			result = send(sock, buffPtr, buffLen, MSG_NOSIGNAL | (more ? MSG_MORE : 0));
			if (result != -1)
				return;
		}
		throw std::runtime_error("Data sending failed: " + std::string(std::strerror(errno)));
	}
}
