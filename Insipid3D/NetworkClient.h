#pragma once
#include <iostream>
//#include <boost/beast/core.hpp>
//#include <boost/beast/websocket.hpp>
//#include <boost/asio/strand.hpp>
//
//namespace beast = boost::beast;         // from <boost/beast.hpp>
//namespace http = beast::http;           // from <boost/beast/http.hpp>
//namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
//namespace net = boost::asio;            // from <boost/asio.hpp>
//using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

struct Engine;

class NetworkClient
{
public:
	NetworkClient(Engine* engine);
	//void connect(std::string address);
private:
	Engine* engine;
	//net::io_context ioc;
	//std::thread ioc_thread;
};