#pragma once
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

using namespace std;

class TCPclient {
public:
	TCPclient();
	~TCPclient() {};

	// Метод настройки клиента
	bool setup(PCSTR addr, PCSTR port);

	// Метод отправки данных
	bool send_data(const string& data);

	// Метод чтения данных от сервера
	void read_data();

	// Метод для запуска клиента в отдельном потоке
	static void run_client(const char* serverAddress, const char* serverPort, string messageToSend, int number);

private:
	ADDRINFO* addrResult;	// Указатель на информацию об адресах
	SOCKET ConnectSocket;	// Сокет
	int result;				// Переменная для хранения результата операций 

	// Метод для очистки ресурсов	
	void clean_up();		
};