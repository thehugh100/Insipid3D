#include "Client_UDP.h"

#include "engine.h"
#include <boost/bind.hpp>
#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::udp;

Client_UDP::Client_UDP(Engine* engine)
	:
	engine(engine),
	recvbuffer(),
	socket_(io_service)
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
				boost::asio::ip::udp::resolver resolver(io_service);
				boost::asio::ip::udp::resolver::query query(boost::asio::ip::udp::v4(), lastServer, port);
				remoteEndpoint = *resolver.resolve(query);

				std::cout << "Created endpoint" << std::endl;

				//if (socket_->is_open())
				//{
				//	std::cout << "Closing opened socket" << std::endl;
				//	socket_->close();
				//}

				socket_.open(boost::asio::ip::udp::v4());

				std::cout << "Opened new socket" << std::endl;

				//io_service->stop();

				std::cout << "Started io_service" << std::endl;

				socket_.send_to(boost::asio::buffer("This is a test message."), remoteEndpoint);
				socket_.send_to(boost::asio::buffer("This is a second test message."), remoteEndpoint);

				std::cout << "Sent initial message" << std::endl;

				start_receive();

				std::cout << "After start_receive" << std::endl;

				io_service.run();

			}
			catch (std::exception& e)
			{
				std::cerr << "Socket error: " << e.what() << std::endl;
			}
		}
	);

	clientThread.detach();
}

void Client_UDP::start_receive()
{
	socket_.async_receive_from(
		boost::asio::buffer(recvbuffer), remoteEndpoint,
		boost::bind(&Client_UDP::handle_receive, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
}

void Client_UDP::handle_receive(const boost::system::error_code& error, std::size_t bytes)
{
	if (!error || error == boost::asio::error::message_size)
	{
		std::cout << "Data: " << recvbuffer.data() << std::endl;
	}
	else// if (error != boost::asio::error::invalid_argument)
	{
		std::cout << "Receive error: " << error.message() << std::endl;
	}

	start_receive();
}

void Client_UDP::tick(float deltaTime)
{
}
