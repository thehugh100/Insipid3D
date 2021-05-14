#include "NetworkClient.h"

#include "engine.h"

NetworkClient::NetworkClient(Engine* engine)
	: 
	udpClient(engine),
	tcpClient(engine)
{
}

void NetworkClient::connect(std::string address)
{
	udpClient.connect(address);
	tcpClient.connect(address);
}

void NetworkClient::tick(float deltaTime)
{
	tcpClient.tick(deltaTime);
	udpClient.tick(deltaTime);
}
