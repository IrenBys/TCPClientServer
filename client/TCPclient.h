#pragma once
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>

using namespace std;

class TCPclient {
private:
	ADDRINFO* addrResult;
	SOCKET ConnectSocket;
	int result;
	void clean_up();
public:
	TCPclient();
	~TCPclient() {};
	bool setup(PCSTR addr, PCSTR port);
	bool send_data(const char* data);
	void read_data();
};