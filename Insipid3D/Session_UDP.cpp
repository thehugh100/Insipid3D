#include "Session_UDP.h"
#include "Server_UDP.h"
#include <iostream>

Session_UDP::Session_UDP(boost::asio::ip::udp::endpoint endpoint, boost::asio::ip::udp::socket *socket, Server_UDP* server_udp)
	:endpoint(endpoint), socket(socket), server_udp(server_udp)
{
	std::cout << "[" << endpoint.address().to_string() << ":" << endpoint.port() << "]: Client Connected." << std::endl;
}

void Session_UDP::receiveData(std::string data)
{
	std::cout << "[" << endpoint.address().to_string() << ":" << endpoint.port() << "]: Received: " << data << std::endl;
}
