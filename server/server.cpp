// server.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "TCPserver.h"

int main() {

	TCPserver tcp;

	tcp.setup("8888");
	tcp.receive("Hello from server");

	return 0;
}

