#pragma once

#include "Client_TCP.h"
#include "Client_UDP.h"

struct Engine;

class NetworkClient
{
public:

	NetworkClient(Engine* engine);

	void connect(std::string address);

	void tick(float deltaTime);

private:

	Client_UDP udpClient;
	Client_TCP tcpClient;

};