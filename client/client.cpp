#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <mutex>
#include "TCPclient.h"

using namespace std;




int main() {

	const char* serverAddress = "localhost";	// Адрес сервера 
	const char* serverPort = "8888";			// Порт сервера
	const int numberOfClients = 5;				// Количество клиентов, которые мы хотим создать
	vector<thread> clients;						// Вектор для хранения потоков клиентов
	
	for(int i = 0; i < numberOfClients; ++i)
	{
		string messageToSend = "Hello Server from client " + to_string(i);
		clients.emplace_back(TCPclient::run_client, serverAddress, serverPort, messageToSend); // Запускаем каждый клиент в отдельном потоке
	}

	// Ждем, пока все клиенты завершатся
	for(auto& client : clients) {
		client.join();
	}

	return 0;
}