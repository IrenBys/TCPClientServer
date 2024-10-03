#include "TCPserver.h"

TCPserver::TCPserver()
{
	addrResult = NULL;
	ClientSocket = INVALID_SOCKET;
	ListenSocket = INVALID_SOCKET;
	result = -1;
}

void TCPserver::clean_up(SOCKET sock)
{
	closesocket(sock);
	freeaddrinfo(addrResult); // Free address information that the getaddrinfo function dynamically allocates in addrinfo structures.
	WSACleanup(); // Terminate use of the Winsock2 DLL (Ws2_32.dll)
}

bool TCPserver::setup(PCSTR port)
{
	WSADATA wsaData; // The structure contains information about the Windows Sockets implementation
	ADDRINFO hints; // The structure is used by the getaddrinfo function to hold host address information

	// Fill hints structure
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Initiate use of the Winsock DLL by a process
	result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if(result != 0)
	{
		cout << "WSAStartup failed, result = " << result << endl;
		return false;
	}

	// Provide protocol-independent translation from an ANSI host name to an address
	result = getaddrinfo(NULL, port, &hints, &addrResult);
	if(result != 0)
	{
		cout << "getaddrinfo failed, result = " << result << endl;
		WSACleanup();
		return false;
	}

	// Create socket
	ListenSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
	if(ListenSocket == INVALID_SOCKET)
	{
		cout << "Socket creation failed" << endl;
		freeaddrinfo(addrResult); 
		WSACleanup(); 
		return false;
	}

	// Binding socket
	result = bind(ListenSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);
	if(result == SOCKET_ERROR)
	{
		cout << "Binding socket failed" << endl;
		ListenSocket = INVALID_SOCKET;
		clean_up(ListenSocket);
		return false;
	}

	// Listening mode (wait info from socket)
	result = listen(ListenSocket, SOMAXCONN);
	if(result == SOCKET_ERROR)
	{
		cout << "Listeting socket failed" << endl;
		clean_up(ListenSocket);
		return false;
	}

	// Return client socket
	ClientSocket = accept(ListenSocket, NULL, NULL);
	if(ClientSocket == INVALID_SOCKET)
	{
		cout << "Accepting socket failed" << endl;
		clean_up(ListenSocket);
		return false;
	}
	closesocket(ListenSocket);
	return true;
}

bool TCPserver::send_data(const char* data)
{
	result = send(ClientSocket, data, (int)strlen(data), 0);
	if(result == SOCKET_ERROR)
	{
		cout << "Send failed, error" << result << endl;
		clean_up(ClientSocket);
		return false;
	}
	return true;
}

bool TCPserver::read_data(const char* data)
{
	const int size_buffer = 512;
	char recvBuffer[size_buffer];

	do {
		ZeroMemory(recvBuffer, sizeof(recvBuffer));
		result = recv(ClientSocket, recvBuffer, 512, 0);
		if(result > 0)
		{

			cout << "Received " << result << " bytes" << endl;
			cout << "Received data: " << recvBuffer << endl;
			send_data(data);
		}
		else if(result == 0)
		{
			cout << "Connection closing..." << endl;
		}
		else
		{
			cout << "recv failed with error" << endl;
			clean_up(ClientSocket);
			return false;
		}
	} while(result > 0);

	result = shutdown(ClientSocket, SD_SEND);
	if(result == SOCKET_ERROR)
	{
		cout << "Shutdown failed, error" << result << endl;
		clean_up(ClientSocket);
		return false;
	}

	clean_up(ClientSocket);
	return true;

	
}
