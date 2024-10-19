// server.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "TCPserver.h"

int main() {

	// Создаем объект сервера
	TCPserver server;

	// Порт, на котором сервер будет слушать
	const char* port = "8888";

	// Настройка сервера на указанный порт
	if(!server.setup(port)) {
		std::cout << "Server setup failed" << std::endl; // Сообщение об ошибке настройки
		return 1; 
	}

	std::cout << "Server is set up and listening on port " << port << std::endl;


	return 0;
}

