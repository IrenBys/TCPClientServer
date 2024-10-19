#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>

class TCPserver {
public:
	TCPserver();
	~TCPserver() {};

	// Метод для настройки сервера
	bool setup(PCSTR port);

private:
	ADDRINFO* addrResult;	// Указатель на информацию об адресах
	SOCKET ClientSocket;
	SOCKET ListenSocket;	// Сокет для прослушивания
	int result;				// Переменная для хранения результата операций

	// Метод для очистки ресурсов
	void clean_up(SOCKET sock);

	// Функция для обработки клиента
	static void client_handler(SOCKET clientSocket);

};

