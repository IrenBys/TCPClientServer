#include "TCPserver.h"
#include <vector>
#include <thread>
#include <mutex>

using namespace std;

// Глобальный мьютекс для обеспечения потокобезопасности при выводе в консоль
mutex coutMutex;

void TCPserver::client_handler(SOCKET clientSocket)
{
	const int size_buffer = 512;	// Размер буфера для приема данных
	char recvBuffer[size_buffer];	// Буфер для хранения принятых данных
	int result;

	// Цикл для приема данных от клиента
	do {
		ZeroMemory(recvBuffer, sizeof(recvBuffer));					// Обнуляем буфер перед каждым вызовом recv
		result = recv(clientSocket, recvBuffer, size_buffer, 0);	// Получаем данные от клиента
		if(result > 0)
		{
			// Проверяем, что результат не превышает размер буфера, прежде чем добавлять нулевой символ
			if(result < size_buffer) {
				recvBuffer[result] = '\0'; // Добавляем нулевой символ для завершения строки
			}
			else {
				recvBuffer[size_buffer - 1] = '\0'; // Если результат равен размеру буфера, завершаем строку на последнем символе
			}
			// Вывод принятых данных
			lock_guard<std::mutex> lock(coutMutex);
			cout << "Received " << result << " bytes" << endl;
			cout << "Received data: " << recvBuffer << endl;
			const char* data = "Message received";
			// Отправляем данные через сокет клиента
			send(clientSocket, data, (int)strlen(data), 0);
		}
		else if(result == 0)
		{
			lock_guard<std::mutex> lock(coutMutex);
			cout << "Connection closing..." << endl;	// Клиент закрыл соединение
		}
		else
		{
			lock_guard<std::mutex> lock(coutMutex);
			cout << "recv failed with error" << endl;
		}
	} while(result > 0); // Продолжаем принимать данные, пока клиент не завершит соединение
	closesocket(clientSocket);
}

// Конструктор класса TCPserver, инициализирует Winsock
TCPserver::TCPserver() : addrResult(nullptr), ClientSocket(INVALID_SOCKET), ListenSocket(INVALID_SOCKET), result(0)
{
	WSADATA wsaData; // Структура содержит информацию о реализации Windows Sockets

	// Инициализация использования библиотеки Winsock
	result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if(result != 0)
	{
		cout << "WSAStartup failed, result = " << result << endl;
	}
}

// Метод для очистки ресурсов (закрытие сокетов и завершение работы Winsock)
void TCPserver::clean_up(SOCKET sock)
{
	// Проверяем, является ли сокет валидным, перед тем как его закрыть
	if(sock != INVALID_SOCKET)
	{
		closesocket(sock);
	}
	// Освобождаем память, выделенную под информацию об адресах
	if(addrResult != nullptr)
	{
		freeaddrinfo(addrResult); // Освобождаем структуру addrinfo, полученную с помощью getaddrinfo
	}

	WSACleanup(); // Завершаем использование библиотеки Winsock
}

// Метод настройки сервера на указанный порт
bool TCPserver::setup(PCSTR port)
{
	
	ADDRINFO hints;	// Структура для хранения информации о сервере (настройки сокета)

	// Инициализируем структуру hints
	ZeroMemory(&hints, sizeof(hints));	
	hints.ai_family = AF_INET;			// Используем IPv4 
	hints.ai_socktype = SOCK_STREAM;	// Используем потоковый сокет (TCP)
	hints.ai_protocol = IPPROTO_TCP;	// Протокол TCP
	hints.ai_flags = AI_PASSIVE;		// Сервер будет работать на всех доступных сетевых интерфейсах

	// Преобразуем имя узла в IP-адрес с помощью getaddrinfo
	result = getaddrinfo(NULL, port, &hints, &addrResult);
	if(result != 0)
	{
		cout << "getaddrinfo failed, result = " << result << endl;
		WSACleanup();
		return false;
	}

	cout << "getaddrinfo is setup" << endl;

	// Создаем сокет для прослушивания входящих соединений
	ListenSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
	if(ListenSocket == INVALID_SOCKET)
	{
		cout << "Socket creation failed" << endl;
		freeaddrinfo(addrResult); 
		WSACleanup(); 
		return false;
	}

	cout << "ListenSocket is created" << endl;

	// Привязываем сокет к адресу и порту, чтобы сервер знал, где принимать соединения
	result = bind(ListenSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);
	if(result == SOCKET_ERROR)
	{
		cout << "Binding socket failed" << WSAGetLastError() << endl;
		ListenSocket = INVALID_SOCKET;
		clean_up(ListenSocket);
		return false;
	}

	cout << "ListenSocket is binded" << endl;

	// Переводим сокет в режим прослушивания (ожидание входящих подключений)
	result = listen(ListenSocket, SOMAXCONN);
	if(result == SOCKET_ERROR)
	{
		cout << "Listeting socket failed" << WSAGetLastError() << endl;
		clean_up(ListenSocket);
		return false;
	}

	cout << "Server is listening on port " << port << endl;

	// Бесконечный цикл для приема входящих подключений

	while(true)
	{
		ClientSocket = accept(ListenSocket, NULL, NULL);	// Принимаем входящее подключение от клиента
		if(ClientSocket == INVALID_SOCKET)
		{
			cout << "Accepting socket failed" << WSAGetLastError() << endl;
			clean_up(ListenSocket);
			return false;
		}

		std::lock_guard<std::mutex> lock(coutMutex);
		cout << "Client connected!" << endl;

		// Создание нового потока для обработки клиента
		thread(client_handler, ClientSocket).detach(); // Отделение потока для обработки клиента

	}
	closesocket(ListenSocket);	// Закрываем сокет прослушивания, так как он больше не нужен
	return true;
}


