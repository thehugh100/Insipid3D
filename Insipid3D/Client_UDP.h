#pragma once
#include <iostream>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/strand.hpp>

#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/write.hpp>

#include <boost/array.hpp>

#include <functional>

using udp = boost::asio::ip::udp;       // from <boost/asio/ip/tcp.hpp>

#include <string>

#include <nlohmann/json.hpp>
#include "EntityClientCam.h"

#include <mutex>
#include <queue>
#include <list>

#include "ClientUtils.h"

struct Engine;


class Client_UDP
{
public:
	Client_UDP(Engine* engine);

	~Client_UDP();

    void connect(std::string address);

	void start_receive();

	void handle_receive(const boost::system::error_code& error, std::size_t bytes);

	void tick(float deltaTime);

	void processMessage(std::string messageData);
private:

    Engine* engine;

	boost::array<char, 65536> recvbuffer;
	boost::asio::ip::udp::endpoint remoteEndpoint;
	boost::asio::io_service io_service;
	boost::asio::ip::udp::socket socket_;


	std::map<std::string, std::function<void(nlohmann::json)>> onCommands;

    std::string lastServer;
    std::string username;
    std::string port = "32500";


	std::map<std::string, EntityClientCam*> clientEntities;


	float updateTimer = 0.0f;
	bool active = true;

	ClientCommand sendClientUpdate;
};