#include "TCPclient.h"

TCPclient::TCPclient()
{
	addrResult = NULL;
	ConnectSocket = INVALID_SOCKET;
	result = -1;
}


void TCPclient::clean_up()
{
	freeaddrinfo(addrResult);
	WSACleanup();
}

bool TCPclient::setup(PCSTR addr, PCSTR port)
{
	WSADATA wsaData; //The structure contains information about the Windows Sockets implementation
	ADDRINFO hints; // The structure is used by the getaddrinfo function to hold host address information
	
	// Fill hints structure
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Initiate use of the Winsock DLL by a process
	result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if(result != 0)
	{
		cout << "WSAStartup failed, result = " << result << endl;
		return false;
	}

	// Provide protocol-independent translation from an ANSI host name to an address
	result = getaddrinfo(addr, port, &hints, &addrResult);
	if(result != 0)
	{
		cout << "getaddrinfo failed, result = " << result << endl;
		WSACleanup();
		return false;
	}

	// Create socket
	ConnectSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
	if(ConnectSocket == INVALID_SOCKET)
	{
		cout << "Socket creation failed" << endl;
		clean_up();
		return false;
	}

	// Connect server
	result = connect(ConnectSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);
	if(result == SOCKET_ERROR)
	{
		cout << "Unable connect to server" << endl;
		closesocket(ConnectSocket);
		ConnectSocket = INVALID_SOCKET;
		clean_up();
		return false;
	}
	return true;
}


