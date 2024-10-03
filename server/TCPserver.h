#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>

using namespace std;

class TCPserver {
private:
	ADDRINFO* addrResult;
	SOCKET ClientSocket;
	SOCKET ListenSocket;
	int result;
	void clean_up(SOCKET sock);
public:
	TCPserver();
	~TCPserver() {};
	bool setup(PCSTR port);
	bool send_data(const char* data);
	bool receive(const char* data);
};
