#include "Session_UDP.h"
#include "Server_UDP.h"
#include <iostream>
#include "EntityClientCam.h"
#include "engine.h"
#include "Util.h"

Session_UDP::Session_UDP(boost::asio::ip::udp::endpoint endpoint, boost::asio::ip::udp::socket *socket, Server_UDP* server_udp)
	:endpoint(endpoint), socket(socket), server_udp(server_udp)
{
	std::cout << "[" << endpoint.address().to_string() << ":" << endpoint.port() << "]: Client Connected." << std::endl;
	sendJson({ {"type", "welcome"}, {"data", "nothing"} });

	entityClientCam = new EntityClientCam(glm::vec3(0, 0, 0));
	server_udp->engine->entityManger->addEntity(entityClientCam);
}

void Session_UDP::receiveData(std::string data)
{
	std::cout << "[" << endpoint.address().to_string() << ":" << endpoint.port() << "]: Received: " << data << std::endl;

	nlohmann::json j = nlohmann::json::parse(data);
	if (j["type"] == "clientUpdate")
	{
		entityClientCam->pos = Util::vec3FromString(j["data"]["pos"]);
		entityClientCam->lookVec = Util::vec3FromString(j["data"]["ang"]);
		//"data": {  "pos": vec3, "ang" : vec3 }
	}
}

void Session_UDP::sendData(std::string data)
{
	socket->async_send_to(boost::asio::buffer(data), endpoint,
		boost::bind(&Server_UDP::handle_send, server_udp,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
}

void Session_UDP::sendJson(nlohmann::json jsonData)
{
	sendData(jsonData.dump());
}

void Session_UDP::sendPeers()
{
	nlohmann::json j;
	j["type"] = "clients";

	for (auto& i : server_udp->sessions)
	{
		if (i.second != this)
		{
			j["data"].push_back(
				{
					{"ip", i.first},
					{"pos", Util::printVec3(entityClientCam->pos)},
					{"ang", Util::printVec3(entityClientCam->lookVec)}
				});
		}
	}
	j["data"].push_back(
		{
			{"ip", "0.0.0.0"},
			{"pos", Util::printVec3(server_udp->engine->camera->pos)},
			{"ang", Util::printVec3(server_udp->engine->camera->lookVec)}
		});

	sendJson(j);
}
