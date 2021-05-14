#include "Client_UDP.h"

#include "engine.h"
#include <boost/bind.hpp>
#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>

#include "ClientUtils.h"
#include "EntityExplosiveBarrel.h"
#include "Util.h"

using boost::asio::ip::udp;

namespace
{

	ClientCommand defaultCommand = [](std::string) {};
}

Client_UDP::Client_UDP(Engine* engine_)
	:
	engine(engine_),
	recvbuffer(),
	socket_(io_service),
	sendClientUpdate(defaultCommand)
{

	sendClientUpdate = [&](std::string) {
		nlohmann::json j;
		j["type"] = "clientUpdate";

		nlohmann::json data;
		data["pos"] = Util::printVec3(engine->camera->pos);
		data["ang"] = Util::printVec3(engine->camera->lookVec);

		j["data"] = data;

		if (socket_.is_open())
		{
			socket_.send_to(boost::asio::buffer(j.dump()), remoteEndpoint);
		}
	};

	onCommands["welcome"] = [&](nlohmann::json msg) {
		std::cout << msg.dump(2) << std::endl;
	};

	onCommands["ping"] = [&](nlohmann::json msg) {
		//sendJson(msg);
	};

	onCommands["cping"] = [&](nlohmann::json msg) {
		//uint64_t ts = 0;
		//json_get_object(msg, "ts", ts)
		//{
		//	uint64_t ms = std::chrono::duration_cast<std::chrono::milliseconds>(
		//		std::chrono::system_clock::now().time_since_epoch()
		//		).count();
		//	std::stringstream ss;
		//	ss << (ms - ts) / 2 << "ms";
		//	*engine->console << "Ping returned in " << ss.str() << std::endl;
		//}
	};

	onCommands["entityUpdate"] = [&](nlohmann::json msg) {
		nlohmann::json j;

		//std::cout << msg.dump(2) << std::endl;

		json_get_object(msg, "data", j)
		{
			//engine->entityManger->clear();
			try
			{

				for (auto& i : j)
				{
					std::string type = i["type"];
					//if (i["active"] == 0)
					//	continue;

					//if (type == "EntityPhysicsProp")
					//{
					//	EntityPhysicsProp* e = (EntityPhysicsProp*)engine->entityManger->addEntity(
					//		new EntityPhysicsProp(i["modelName"], Util::vec3FromString(i["origin"]), i["mass"])
					//	);
					//	e->setTransform(Util::mat4FromString(i["transform"]));
					//}
					if (type == "EntityExplosiveBarrel")
					{
						int id = i["id"];


						engine->netEvents->pushInstruction(
							[=]() {


								Entity* foundEntity = engine->entityManger->getEntityByID(id);
								if (foundEntity == nullptr)
								{
									std::cout << "Creating new entity " << id << std::endl;
									EntityExplosiveBarrel* newEntity = (EntityExplosiveBarrel*)engine->entityManger->addEntity(
										new EntityExplosiveBarrel(Util::vec3FromString(i["origin"]))
									);
									if (newEntity != nullptr)
									{
										json_get_string(i, "transform", transform)
										{
											newEntity->setTransform(Util::mat4FromString(transform));
											newEntity->id = id;
										}
									}
								}
								else
								{
									std::cout << "Found entity " << id << std::endl;
									auto e = (EntityExplosiveBarrel*)foundEntity;
									if (e != nullptr && e->body != nullptr)
									{
										json_get_string(i, "transform", transform)
										{
											e->setTransform(Util::mat4FromString(transform));
										}

										json_get_string(i, "velocity", velocity)
										{
											glm::vec3 vec = Util::vec3FromString(velocity);
											btVector3 btVec = Util::vec3Conv(vec);
											e->body->setLinearVelocity(btVec);
										}

										json_get_string(i, "angularVelocity", angularVelocity)
										{
											e->body->setAngularVelocity(Util::vec3Conv(Util::vec3FromString(angularVelocity)));
										}

										int activeState = false;
										json_get_object(i, "active", activeState)
										{
											if (!activeState)
												e->remove();
											e->active = activeState;
										}

										e->body->activate();
									}
								}


								//std::cout << "Created Barrel" << std::endl;
							}
						);
					}
				}
			}
			catch (nlohmann::json::exception& e)
			{
				*engine->console << "message: " << e.what() << '\n'
					<< "exception id: " << e.id << std::endl;
			}
		}
	};

	onCommands["clients"] = [&](nlohmann::json msg) {
		nlohmann::json data;
		json_get_object(msg, "data", data)
		{
			for (auto& clientJson : data)
			{
				json_get_string(clientJson, "ip", username)
				{
					auto& clientEntityItor = clientEntities.find(username);
					if (clientEntityItor != clientEntities.end())
					{
						auto& client = clientEntities[username];

						if (client != nullptr)
						{
							json_get_string(clientJson, "pos", pos)
							{
								engine->netEvents->pushInstruction(
									[=]() {
										if (client != nullptr)
											client->pos = Util::vec3FromString(pos);
									}
								);
							}
						}
					}
					else
					{
						EntityClientCam* newClient = new EntityClientCam(glm::vec3(0));
						json_get_string(clientJson, "pos", pos)
						{
							if (newClient != nullptr)
								newClient->pos = Util::vec3FromString(pos);
						}
						clientEntities[username] = newClient;

						engine->netEvents->pushInstruction(
							[newClient, this]() {
								if (newClient != nullptr)
									this->engine->entityManger->addEntity(newClient);
							}
						);
					}
				}
			}
		}
	};

}

Client_UDP::~Client_UDP()
{
}

void Client_UDP::connect(std::string address)
{
	engine->entityManger->clear();
	clientEntities.clear();

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

				if (socket_.is_open())
				{
					std::cout << "Closing opened socket" << std::endl;
					io_service.stop();
					socket_.close();
				}

				socket_.open(boost::asio::ip::udp::v4());

				std::cout << "Opened new socket" << std::endl;

				//io_service->stop();

				std::cout << "Started io_service" << std::endl;

				socket_.send_to(boost::asio::buffer(nlohmann::json({ { "type", "test" }, { "data", "hello" } }).dump()), remoteEndpoint);
				//socket_.send_to(boost::asio::buffer("This is a second test message."), remoteEndpoint);

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
		//std::cout << "Data: " << recvbuffer.data() << std::endl;
		processMessage(recvbuffer.data());
		recvbuffer = boost::array<char, 65536>();
	}
	else// if (error != boost::asio::error::invalid_argument)
	{
		std::cout << "Receive error: " << error.message() << std::endl;
	}

	start_receive();
}

void Client_UDP::tick(float deltaTime)
{
	updateTimer += deltaTime;

	if (updateTimer > 1.f / 64.0f) //60 ticks a second
	{
		updateTimer = 0;
		if (active)
		{
			sendClientUpdate("");
		}
	}
}

void Client_UDP::processMessage(std::string messageData)
{
	try
	{
		//const std::lock_guard<std::mutex> lock(engine->engineLock);

		nlohmann::json msg = nlohmann::json::parse(messageData);

		//std::cout << "NetworkClient::processMessage() - msg = " << msg.dump(2) << std::endl;

		int tick = 0;
		json_get_object(msg, "tick", tick)
		{
			std::cout << "tick: " << tick << std::endl;
		}

		json_get_string(msg, "type", type)
		{
			auto onCommand = onCommands.find(type);
			if (onCommand != onCommands.end())
			{
				//*engine->console << "[Received] " << type << std::endl;
				onCommands[type](msg);
			}
			else
			{
				*engine->console << "Invalid type recieved: " << type << std::endl;
			}
		}

	}
	catch (nlohmann::json::exception& e)
	{
		*engine->console << e.what() << std::endl;
		*engine->console << messageData << std::endl;
	}
}
