#pragma once

#include <iostream>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/strand.hpp>

#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/write.hpp>
#include <functional>

//namespace beast = boost::beast;         // from <boost/beast.hpp>
//namespace http = beast::http;           // from <boost/beast/http.hpp>
//namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
//namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

#include <string>

#include <nlohmann/json.hpp>
#include "EntityClientCam.h"

#include "ClientUtils.h"

struct Engine;

class Client_TCP
{
public:


	Client_TCP(Engine* engine);


	void connect(std::string address);

	// Called by the user of the client class to initiate the connection process.
	// The endpoints will have been obtained using a tcp::resolver.
	bool start(tcp::resolver::results_type endpoints);

	// Called by the user of the client class to initiate the connection process.
	// The endpoints will have been obtained using a tcp::resolver.
	void restart(bool ask = false);

	// This function terminates all the actors to shut down the connection. It
	// may be called by the user of the client class, or by the class itself in
	// response to graceful termination or an unrecoverable error.
	void stop();

	void disconnect();


	void start_connect(tcp::resolver::results_type::iterator endpoint_iter);


	void handle_connect(const boost::system::error_code& error,
		tcp::resolver::results_type::iterator endpoint_iter);

	void start_read();


	void handle_read(const boost::system::error_code& error, std::size_t n);

	void processMessage(std::string messageData);


	void writePacket(std::string response);

	void writePacket(boost::asio::const_buffer response);

	void handle_write(const boost::system::error_code& error);

	void sendJson(nlohmann::json j);

	void processWelcome(nlohmann::json data);

	ClientCommand requestPing;
	ClientCommand sendClientUpdate;

	void tick(float deltaTime);


	std::map<std::string, std::function<void(nlohmann::json)>> onCommands;

private:

	float updateTimer = 0.0f;
	bool active = true;

	boost::asio::io_context io_context;
	tcp::resolver r;

	Engine* engine;

	std::string lastServer;
	std::string username;
	std::string port = "32500";

	bool stopped_ = false;
	tcp::resolver::results_type endpoints_;
	tcp::socket socket_;

	std::mutex receiveMutex, readMutex;
	std::mutex writeMutex;

	const size_t max_length = 4096;
	char data_[4096];

	const size_t packet_body_length = 4096;
	char packet_body[4096];

	uint32_t expectedLength = 4;
	uint32_t messageCounter = 0;

	std::chrono::high_resolution_clock::time_point lastBytesSentTS;
	size_t lastBytesSent;

	std::map<std::string, EntityClientCam*> clientEntities;
};
