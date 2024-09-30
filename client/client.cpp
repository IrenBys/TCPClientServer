#include "TCPclient.h"

int main() {
	
	TCPclient tcp;

	tcp.setup("localhost", "8888");

	return 0;
}