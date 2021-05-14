#pragma once
#include <iostream>
#include <boost/asio.hpp>

struct Engine;
class server_tcp;
class Server_UDP;

class NetworkServer
{
public:
	NetworkServer(Engine* engine);
	void startServer();
	void stopServer();
	void tick(float deltaTime);
	server_tcp* server_tcp_;
	Server_UDP* server_udp_;
	boost::asio::io_context io_context;
	int port;
	uint64_t serverTicks;
private:
	Engine* engine;
	float updateTimer;
	bool active;
};