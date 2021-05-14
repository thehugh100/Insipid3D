#include "Client_TCP.h"
#include "engine.h"
#include "NetEvents.h"

//#include <boost/beast/core.hpp>
//#include <boost/beast/websocket.hpp>
//#include <boost/asio/strand.hpp>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <sstream>

#include <nlohmann/json.hpp>

//namespace beast = boost::beast;         // from <boost/beast.hpp>
//namespace http = beast::http;           // from <boost/beast/http.hpp>
//namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
//namespace net = boost::asio;            // from <boost/asio.hpp>
//using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
//

#include "EntityPhysicsProp.h"
#include "EntityExplosiveBarrel.h"
#include "Util.h"


#include "ClientUtils.h"

using std::placeholders::_1;
using std::placeholders::_2;



namespace
{
	const std::string SCHEMA_TYPE = "type";
	const std::string SCHEMA_DATA = "data";
	const std::string SCHEMA_AES_KEY = "aes_key";
	const std::string SCHEMA_AES_IV = "aes_iv";
	const std::string SCHEMA_CRYPT = "crypt";
	const std::string SCHEMA_PUBLIC_KEY = "public_key";

	const std::string SCHEMA_TYPE__RSA_PUB = "RSA_PUB";
	const std::string SCHEMA_TYPE__WELCOME = "welcome";
	const std::string SCHEMA_TYPE__ECHO = "echo";
	const std::string SCHEMA_TYPE__ANNOUNCE = "announce";
	const std::string SCHEMA_TYPE__CRYPT = "crypt";

	ClientCommand defaultCommand = [](std::string) {};
}



Client_TCP::Client_TCP(Engine* engine_)
	:
	engine(engine_),
	r(io_context),
	socket_(io_context),
	data_(),
	packet_body()
{
	requestPing = [&](std::string input) {
		Args args = extractLiteralArgs(input);
		if (args.size() == 0)
		{
			nlohmann::json j;
			j["type"] = "cping";
			uint64_t ms = std::chrono::duration_cast<std::chrono::milliseconds>(
				std::chrono::system_clock::now().time_since_epoch()
				).count();
			j["ts"] = ms;

			sendJson(j);
		}
		else
		{
			*engine->console << "usage: ping" << std::endl;
		}
	};

	sendClientUpdate = [&](std::string) {
		nlohmann::json j;
		j["type"] = "clientUpdate";

		nlohmann::json data;
		data["pos"] = Util::printVec3(engine->camera->pos);
		data["ang"] = Util::printVec3(engine->camera->ang);

		j["data"] = data;

		sendJson(j);
	};

	onCommands["welcome"] = [&](nlohmann::json msg) {
		processWelcome(msg);
	};

	onCommands["ping"] = [&](nlohmann::json msg) {
		sendJson(msg);
	};

	onCommands["cping"] = [&](nlohmann::json msg) {
		uint64_t ts = 0;
		json_get_object(msg, "ts", ts)
		{
			uint64_t ms = std::chrono::duration_cast<std::chrono::milliseconds>(
				std::chrono::system_clock::now().time_since_epoch()
				).count();
			std::stringstream ss;
			ss << (ms - ts) / 2 << "ms";
			*engine->console << "Ping returned in " << ss.str() << std::endl;
		}
	};

	onCommands["entityUpdate"] = [&](nlohmann::json msg) {
		nlohmann::json j;
		const std::lock_guard<std::mutex> lock(engine->engineLock);
		//std::cout << msg.dump(2) << std::endl;

		json_get_object(msg, SCHEMA_DATA, j)
		{
			//engine->entityManger->clear();
			try
			{

				for (auto& i : j)
				{
					std::string type = i["type"];
					if (i["active"] == 0)
						continue;

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
									newEntity->setTransform(Util::mat4FromString(i["transform"]));
									newEntity->id = id;
								}
								else
								{
									std::cout << "Found entity " << id << std::endl;

									((EntityExplosiveBarrel*)foundEntity)->setTransform(Util::mat4FromString(i["transform"]));
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
		json_get_object(msg, SCHEMA_DATA, data)
		{
			for (auto& clientJson : data)
			{
				json_get_string(clientJson, "ip", username)
				{
					auto& clientEntityItor = clientEntities.find(username);
					if (clientEntityItor != clientEntities.end())
					{
						auto& client = clientEntities[username];

						if (client)
						{
							json_get_string(clientJson, "pos", pos)
							{
								engine->netEvents->pushInstruction(
									[=]() {
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
							newClient->pos = Util::vec3FromString(pos);
						}
						clientEntities[username] = newClient;

						engine->netEvents->pushInstruction(
							[newClient, this]() {
								this->engine->entityManger->addEntity(newClient);
							}
						);
					}
				}
			}
		}
	};
}

void Client_TCP::connect(std::string address)
{
	engine->entityManger->clear();
	clientEntities.clear();
	//delete engine->map;
	//engine->map = nullptr;

	std::thread clientThread;

	lastServer = address;
	port = "32500";
	if (address == "")
	{
		lastServer = "81.147.31.211";
	}

	clientThread = std::thread([this]() {
		try {
			*engine->console << lastServer << ":" << port << std::endl;
			tcp::resolver::query query(lastServer, port);
			auto endpoint_iterator = r.resolve(query);
			if (start(endpoint_iterator))
			{
				io_context.restart();
				io_context.run();
			}
		}
		catch (std::exception& e)
		{
			std::cerr << "NetClient Exception: " << e.what() << "\n";
		}
		});

	clientThread.detach();
}

bool Client_TCP::start(tcp::resolver::results_type endpoints)
{
	bool success = false;

	endpoints_ = endpoints;

	restart();

	success = true;

	return success;
}

void Client_TCP::restart(bool ask)
{
	bool success = true;

	if (success)
	{
		stopped_ = false;
		start_connect(endpoints_.begin());
	}
}

void Client_TCP::stop()
{
	stopped_ = true;
	boost::system::error_code ignored_error;
	socket_.close(ignored_error);
}

void Client_TCP::disconnect()
{
	boost::system::error_code ignored_error;
	socket_.close(ignored_error);
}

void Client_TCP::start_connect(tcp::resolver::results_type::iterator endpoint_iter)
{
	if (endpoint_iter != endpoints_.end())
	{
		*engine->console << "Connecting on " << endpoint_iter->endpoint() << "..." << std::endl;

		// Start the asynchronous connect operation.
		socket_.async_connect(endpoint_iter->endpoint(),
			std::bind(&Client_TCP::handle_connect,
				this, _1, endpoint_iter));
	}
}

void Client_TCP::handle_connect(const boost::system::error_code& error, tcp::resolver::results_type::iterator endpoint_iter)
{
	if (stopped_)
		return;

	// The async_connect() function automatically opens the socket at the start
	// of the asynchronous operation. If the socket is closed at this time then
	// the timeout handler must have run first.
	if (!socket_.is_open())
	{
		*engine->console << "Connect timed out" << std::endl;

		// Try the next available endpoint.
		start_connect(++endpoint_iter);

		stop();
	}

	// Check if the connect operation failed before the deadline expired.
	else if (error)
	{
		*engine->console << "Connect error: " << error.message() << std::endl;

		// We need to close the socket used in the previous connection attempt
		// before starting a new one.
		socket_.close();

		// Try the next available endpoint.
		start_connect(++endpoint_iter);

		stop();
	}

	// Otherwise we have successfully established a connection.
	else
	{
		*engine->console << "Connected as " << username << " on " << endpoint_iter->endpoint() << std::endl;
		//socket_.set_option(boost::asio::ip::tcp::no_delay(true));
		// Start the input actor.
		start_read();
	}
}

void Client_TCP::start_read()
{
	const std::lock_guard<std::mutex> lock(receiveMutex);
	//*engine->console << "start_read enter" << std::endl;
	//read start of packet
	boost::asio::async_read(socket_, boost::asio::buffer(data_, 4), std::bind(&Client_TCP::handle_read, this, _1, _2));
}

void Client_TCP::handle_read(const boost::system::error_code& error, std::size_t n)
{
	const std::lock_guard<std::mutex> lock(readMutex);
	//*engine->console << "handle_read enter" << std::endl;
	if (!error)
	{
		uint32_t dataSize = 0;
		memcpy(&dataSize, data_, 4);

		//*engine->console << "size was : " << dataSize << " max size : " << packet_body_length << std::endl;

		if (dataSize > packet_body_length) {
			//big problem, packet too big
			*engine->console << "big problem, packet too big - size was: " << dataSize << " max size: " << packet_body_length << std::endl;
		}

		boost::asio::read(socket_, boost::asio::buffer(packet_body, dataSize));

		std::string message = std::string(packet_body, dataSize);

		processMessage(message);

		start_read();
	}
	else
	{
		*engine->console << "Error on receive: " << error.message() << std::endl;

		stop();
	}

}

void Client_TCP::processMessage(std::string messageData)
{
	try
	{
		nlohmann::json msg = nlohmann::json::parse(messageData);

		//std::cout << "NetworkClient::processMessage() - msg = " << msg.dump(2) << std::endl;

		json_get_string(msg, SCHEMA_TYPE, type)
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

void Client_TCP::processWelcome(nlohmann::json data)
{
	const std::lock_guard<std::mutex> lock(engine->engineLock);
	//*engine->console << "NetworkClient::processWelcome() - data = " << data << std::endl;

	//std::cout << data.dump(4) << std::endl;

	//if (data["data"]["map"] != "")
	//{
	//	//engine->loadMap(data["data"]["map"]);
	//}

	//try
	//{
	//	std::cout << "trace 1" << std::endl;

	//	nlohmann::json j = data["data"]["entities"];

	//	std::cout << "trace 2" << std::endl;

	//	for (auto& i : j)
	//	{
	//		std::cout << "trace 3" << std::endl;

	//		std::string type = i["type"];

	//		std::cout << "trace 4" << std::endl;

	//		if (i["active"] == 0)
	//			continue;

	//		std::cout << "trace 5" << std::endl;



	//		/*if (type == "EntityLight")
	//			engine->getMap()->addLight(new Light(Util::vec3FromString(i["point"]),
	//				Util::vec3FromString(i["light_col"]),
	//				i["light_intensity"],
	//				i["light_type"],
	//				Util::vec3FromString(i["light_dir"])
	//			));*/
	//			//if (type == "EntityPhysicsProp")
	//			//{
	//			//	std::function<void()> ins = [&](){
	//			//		EntityPhysicsProp* e = (EntityPhysicsProp*)engine->entityManger->addEntityNoInit(
	//			//			new EntityPhysicsProp(i["modelName"], Util::vec3FromString(i["origin"]), i["mass"])
	//			//		);
	//			//		e->setTransform(Util::mat4FromString(i["transform"]));
	//			//	};
	//			//}
	//		if (type == "EntityExplosiveBarrel")
	//		{
	//			std::cout << "trace 6" << std::endl;

	//			engine->netEvents->pushInstruction(
	//				[=]() {
	//					EntityExplosiveBarrel* e = (EntityExplosiveBarrel*)engine->entityManger->addEntity(
	//						new EntityExplosiveBarrel(Util::vec3FromString(i["origin"]))
	//					);
	//					e->setTransform(Util::mat4FromString(i["transform"]));

	//					std::cout << "Created Barrel" << std::endl;
	//				}
	//			);

	//			std::cout << "trace 7" << std::endl;


	//		}
	//	}
	//}
	//catch (nlohmann::json::exception& e)
	//{
	//	*engine->console << "message: " << e.what() << '\n'
	//		<< "exception id: " << e.id << std::endl;
	//}
}

void Client_TCP::tick(float deltaTime)
{
	updateTimer += deltaTime;

	if (updateTimer > 1.f / 32.0f) //60 ticks a second
	{
		updateTimer = 0;
		if (active)
		{
			sendClientUpdate("");
		}
	}
}

void Client_TCP::writePacket(std::string response)
{
	writePacket(boost::asio::const_buffer(response.c_str(), response.length()));
}

void Client_TCP::writePacket(boost::asio::const_buffer response)
{
	const std::lock_guard<std::mutex> lock(writeMutex);

	uint32_t totalSize = response.size() + 4;
	char* packet = new char[totalSize];

	uint32_t size = response.size();
	memcpy(&packet[0], &size, 4);
	memcpy(&packet[4], response.data(), size);

	boost::asio::async_write(socket_, boost::asio::buffer(packet, totalSize),
		std::bind(&Client_TCP::handle_write, this, _1));
}

void Client_TCP::handle_write(const boost::system::error_code& error)
{
	if (stopped_)
		return;

	if (!error)
	{
	}
	else
	{
		//*engine->console << "Error on writing: " << error.message() << std::endl;
		//stop();
	}
}

void Client_TCP::sendJson(nlohmann::json j)
{
	writePacket(j.dump());
}
