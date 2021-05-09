#pragma once
#include <iostream>
#include <boost/asio.hpp>

struct Engine;
class server;

class NetworkServer
{
public:
	NetworkServer(Engine* engine);
	void startServer();
	void stopServer();
	void tick(float deltaTime);
	server *server_;
	boost::asio::io_context io_context;
	int port;
	uint64_t serverTicks;
private:
	Engine* engine;
	float updateTimer;
	bool active;
};