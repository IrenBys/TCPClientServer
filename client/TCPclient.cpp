#include <iostream>
#include <mutex>
#include "TCPclient.h"

using namespace std;

mutex cout_mutex; // Глобальный мьютекс для синхронизации вывода

void TCPclient::run_client(const char* serverAddress, const char* serverPort, string messageToSend, int number) {

	TCPclient client;	// Создаем объект клиента

	// Настройка клиента
	if(!client.setup(serverAddress, serverPort)) {
		lock_guard<mutex> lock(cout_mutex);
		cerr << "Client setup failed." << endl;
		return; // Выход с ошибкой
	}

	{
		lock_guard<mutex> lock(cout_mutex);
		cout << "Client " << number
			 << " is set up and ready to send data on port " << serverPort << endl;
	}

	// Отправка данных на сервер
	if(!client.send_data(messageToSend)) {
		lock_guard<mutex> lock(cout_mutex);
		cerr << "Error sending data to the server." << endl;
		return; // Выход с ошибкой
	}

	{
		lock_guard<mutex> lock(cout_mutex);
		cout << "Client " << number
			<< " connecting to " << serverAddress
			<< " on port " << serverPort
			<< " with message: " << messageToSend << endl;
	}
	// Чтение ответа от сервера
	client.read_data();
}


TCPclient::TCPclient() : addrResult(nullptr), ConnectSocket(INVALID_SOCKET), result(0) {

	WSADATA wsaData; // Структура содержит информацию о реализации Windows Sockets

	// Инициализация использования библиотеки Winsock
	result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if(result != 0)	{
		lock_guard<mutex> lock(cout_mutex);
		cout << "WSAStartup failed, result = " << result << endl;
	}
}

// Метод для очистки ресурсов (закрытие сокетов и завершение работы Winsock)
void TCPclient::clean_up()
{
	// Закрываем сокет подключения, если он валиден
	if(ConnectSocket != INVALID_SOCKET) {
		closesocket(ConnectSocket);
	}
	// Освобождаем память, выделенную под информацию об адресах
	if(addrResult != nullptr) {
		freeaddrinfo(addrResult); // Освобождаем структуру addrinfo, полученную с помощью getaddrinfo
	}
	// Завершаем использование библиотеки Winsock
	WSACleanup(); // Завершаем работу Winsock
}

bool TCPclient::setup(PCSTR addr, PCSTR port)
{
	ADDRINFO hints; // Структура используется функцией getaddrinfo для хранения информации об адресе
	
	// Заполняем структуру hints
	ZeroMemory(&hints, sizeof(hints));	
	hints.ai_family = AF_INET;			// Используем IPv4
	hints.ai_socktype = SOCK_STREAM;	// Используем потоковый сокет (TCP)
	hints.ai_protocol = IPPROTO_TCP;	// Протокол TCP

	// Получаем адреса для подключения к серверу
	result = getaddrinfo(addr, port, &hints, &addrResult);
	if(result != 0)	{
		lock_guard<mutex> lock(cout_mutex);
		cout << "getaddrinfo failed, result = " << result << endl;
		WSACleanup();
		return false;
	}

	// Создаем сокет подключения
	ConnectSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
	if(ConnectSocket == INVALID_SOCKET)	{
		lock_guard<mutex> lock(cout_mutex);
		cout << "Socket creation failed, error = " << WSAGetLastError() << endl;
		freeaddrinfo(addrResult);	// Освобождаем память
		WSACleanup();				// Завершаем работу Winsock
		return false;
	}

	// Connect server
	result = connect(ConnectSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);
	if(result == SOCKET_ERROR) {
		lock_guard<mutex> lock(cout_mutex);
		cout << "Unable connect to server, error = " << WSAGetLastError() << endl;
		ConnectSocket = INVALID_SOCKET;
		clean_up();
		return false;
	}
	return true; // Успешное подключение
}

// Метод отправки данных на сервер
bool TCPclient::send_data(const string& data)
{
	// Отправляем данные через сокет подключения
	result = send(ConnectSocket, data.c_str(), (int)data.size(), 0);
	if(result == SOCKET_ERROR) {
		lock_guard<mutex> lock(cout_mutex);
		cout << "Send failed, error = " << result << endl;
		clean_up();
		return false;
	}
	
	result = shutdown(ConnectSocket, SD_SEND);
	if(result == SOCKET_ERROR) {
		lock_guard<mutex> lock(cout_mutex);
		cout << "Shutdown failed, error = " << WSAGetLastError() << endl;
		clean_up();
		return false;
	}
	return true;
}

// Метод чтения данных с сервера
void TCPclient::read_data()
{
	const int size_buffer = 512;	// Размер буфера для приема данных
	char recvBuffer[size_buffer];	// Буфер для хранения принятых данных
	
	// Цикл для приема данных от сервера
	do {
		ZeroMemory(recvBuffer, sizeof(recvBuffer));	// Обнуляем буфер перед каждым вызовом recv
		result = recv(ConnectSocket, recvBuffer, size_buffer, 0);	// Получаем данные от сервера
		if(result > 0) {
			// Проверяем, что результат не превышает размер буфера, прежде чем добавлять нулевой символ
			if(result < size_buffer) {
				recvBuffer[result] = '\0'; // Добавляем нулевой символ для завершения строки
			}
			else {
				recvBuffer[size_buffer - 1] = '\0'; // Если результат равен размеру буфера, завершаем строку на последнем символе
			}
			{
				lock_guard<mutex> lock(cout_mutex);
				cout << "Received " << result << " bytes" << endl;
				cout << "Received data: " << recvBuffer << endl;
			}

		}
		else if(result == 0) {
			lock_guard<mutex> lock(cout_mutex);
			cout << "Connection closed by server" << endl; // Сервер закрыл соединение
		}
		else {
			lock_guard<mutex> lock(cout_mutex);
			cout << "recv failed, error = " << WSAGetLastError() << endl;
		}
	} while(result > 0);	// Продолжаем принимать данные, пока есть данные от сервера

	// Закрываем соединение, поскольку больше данных получать не будем
	clean_up();	// Очищаем ресурсы
}
