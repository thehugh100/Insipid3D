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
#include <functional>

using udp = boost::asio::ip::udp;       // from <boost/asio/ip/tcp.hpp>

#include <string>

#include <nlohmann/json.hpp>
#include "EntityClientCam.h"

#include <mutex>
#include <queue>
#include <list>

struct Engine;


class Client_UDP
{
public:
	Client_UDP(Engine* engine,
		const std::string& host,
		const std::string& port
	);

	~Client_UDP();

    void connect(std::string address);


private:

    Engine* engine;


    std::string lastServer;
    std::string username;
    std::string port = "32500";
};