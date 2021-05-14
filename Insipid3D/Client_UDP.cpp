#include "Client_UDP.h"

#include "engine.h"
#include <boost/bind.hpp>
#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::udp;

Client_UDP::Client_UDP(Engine* engine, const std::string& host, const std::string& port)
	:
	engine(engine),
	lastServer(host),
	port(port)
{

}

Client_UDP::~Client_UDP()
{
}

void Client_UDP::connect(std::string address)
{
	std::thread clientThread;

	lastServer = address;
	port = "32500";
	if (address == "")
	{
		lastServer = "81.147.31.211";
	}

	clientThread = std::thread([this]() 
		{
			try
			{
				boost::asio::io_service io_service;

				udp::resolver resolver(io_service);
				udp::resolver::query query(udp::v4(), lastServer, port);
				udp::endpoint receiver_endpoint = *resolver.resolve(query);

				udp::socket socket(io_service);
				socket.open(udp::v4());

				socket.send_to(boost::asio::buffer("This is a test message."), receiver_endpoint);

				boost::array<char, 65535> recv_buf;
				udp::endpoint sender_endpoint;
				size_t len = socket.receive_from(
					boost::asio::buffer(recv_buf), sender_endpoint);

				std::cout.write(recv_buf.data(), len);
			}
			catch (std::exception& e)
			{
				std::cerr << e.what() << std::endl;
			}
		}
	);

	clientThread.detach();
}
