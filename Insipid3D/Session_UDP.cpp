#include "Session_UDP.h"
#include "Server_UDP.h"
#include <iostream>

Session_UDP::Session_UDP(boost::asio::ip::udp::endpoint endpoint, boost::asio::ip::udp::socket *socket, Server_UDP* server_udp)
	:endpoint(endpoint), socket(socket), server_udp(server_udp)
{
	std::cout << "[" << endpoint.address().to_string() << ":" << endpoint.port() << "]: Client Connected." << std::endl;

	sendJson({ {"type", "welcome"}, {"data", "nothing"} });
}

void Session_UDP::receiveData(std::string data)
{
	std::cout << "[" << endpoint.address().to_string() << ":" << endpoint.port() << "]: Received: " << data << std::endl;
}

void Session_UDP::sendData(std::string data)
{
	socket->async_send_to(boost::asio::buffer(data), endpoint,
		boost::bind(&Server_UDP::handle_send, server_udp,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
}

void Session_UDP::sendJson(nlohmann::json jsonData)
{
	sendData(jsonData.dump());
}
