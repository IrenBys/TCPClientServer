# TCPClientServer

There are two distinct types of socket network applications: Server and Client.

Servers and Clients have different behaviors; therefore, the process of creating them is different. What follows is the general model for creating a streaming TCP/IP Server and Client.

** Server **
* Initialize Winsock.
* Create a socket.
* Bind the socket.
* Listen on the socket for a client.
* Accept a connection from a client.
* Receive and send data.
* Disconnect.
**  Client **
* Initialize Winsock.
* Create a socket.
* Connect to the server.
* Send and receive data.
* Disconnect.
