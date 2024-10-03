#include "TCPclient.h"

int main() {
	
	TCPclient tcp;

	tcp.setup("localhost", "8888");
	tcp.send_data("Hello from client");
	tcp.read_data();

	return 0;
}