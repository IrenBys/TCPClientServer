#include "TCPclient.h"

TCPclient::TCPclient()
{
	addrResult = NULL;
	ConnectSocket = INVALID_SOCKET;
	result = -1;
}

void TCPclient::clean_up()
{
	closesocket(ConnectSocket);
	freeaddrinfo(addrResult); // Free address information that the getaddrinfo function dynamically allocates in addrinfo structures.
	WSACleanup(); // Terminate use of the Winsock2 DLL (Ws2_32.dll)
}

bool TCPclient::setup(PCSTR addr, PCSTR port)
{
	WSADATA wsaData; // The structure contains information about the Windows Sockets implementation
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
		freeaddrinfo(addrResult); // Free address information that the getaddrinfo function dynamically allocates in addrinfo structures.
		WSACleanup(); // Terminate use of the Winsock2 DLL (Ws2_32.dll)
		return false;
	}

	// Connect server
	result = connect(ConnectSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);
	if(result == SOCKET_ERROR)
	{
		cout << "Unable connect to server" << endl;
		ConnectSocket = INVALID_SOCKET;
		clean_up();
		return false;
	}
	return true;
}

bool TCPclient::send_data(const char* data)
{
	result = send(ConnectSocket, data, (int)strlen(data), 0);
	if(result == SOCKET_ERROR)
	{
		cout << "Send failed, error" << result << endl;
		clean_up();
		return false;
	}

	cout << "Send " << result << " bytes" << endl;
	result = shutdown(ConnectSocket, SD_SEND);
	if(result == SOCKET_ERROR)
	{
		cout << "Shutdown failed, error" << result << endl;
		clean_up();
		return false;
	}
	return true;
}

void TCPclient::read_data()
{
	const int size_buffer = 512;
	char recvBuffer[size_buffer];
	
	do {
		ZeroMemory(recvBuffer, sizeof(recvBuffer));
		result = recv(ConnectSocket, recvBuffer, 512, 0);
		if(result > 0)
		{
			
			cout << "Received " << result << " bytes" << endl;
			cout << "Received data: " << recvBuffer << endl;
		}
		else if(result == 0)
		{
			cout << "Connection closed" << endl;
		}
		else
		{
			cout << "recv failed with error" << endl;
		}
	} while(result > 0);

	clean_up();
}
