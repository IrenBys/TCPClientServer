#include <iostream>
#include "TCPserver.h"

using namespace std;

int main() {

	// Создаем объект сервера
	TCPserver server;

	// Порт, на котором сервер будет слушать
	const char* port = "8888";

	// Настройка сервера на указанный порт
	if(!server.setup(port)) {
		cout << "Server setup failed" << endl; 
		return 1; 
	}

	cout << "Server is set up and listening on port " << port << endl;

	return 0;
}
