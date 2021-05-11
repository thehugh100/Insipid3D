#include "session.h"
#include "server.h"

#include <Base64.h>
#include <algorithm>
#include <iomanip>
#include <mutex>
#include "engine.h"
#include "EntityManager.h"
#include "Util.h"

session::session(tcp::socket socket, server* serverPtr)
    : socket_(std::move(socket)), serverPtr(serverPtr), packet_body_length(4096)
{
    entityClientCam = nullptr;
}

void session::start()
{
	//socket_.set_option(tcp::no_delay(true));
    *serverPtr->engine->console << "Client connected: " << socket_.remote_endpoint().address() << std::endl;

    entityClientCam = new EntityClientCam(glm::vec3(0, 0, 0));
    serverPtr->engine->entityManger->addEntity(entityClientCam);

    nlohmann::json msg;
    
    // Type 
    msg["type"] = "welcome";

    // Server message for client consoles
    msg["serverMessage"] = "Welcome to Insipid3D test server.";

    // Form json for the engine data
    nlohmann::json data;

    // Map
    data["map"] = serverPtr->engine->map->getMesh()->filename;

    // Entities
    EntityList entityList;
    serverPtr->engine->entityManger->getAllEntities(&entityList);
    nlohmann::json entities;
    for (auto& entity : entityList)
        entities.push_back(nlohmann::json::parse(entity->serialize()));
    data["entities"] = entities;

    // Add data to the message
    msg["data"] = data;

    std::string data_json = msg.dump();

    do_write(boost::asio::buffer(data_json, data_json.size()));

    sendClientPing();

    //*serverPtr->engine->console << "\tSent Welcome Message" << std::endl;

	lastBytesSentTS = std::chrono::high_resolution_clock::now();
	lastBytesSent = 0;

    do_read();
}

void session::readPacket(boost::asio::const_buffer packet)
{
    uint32_t dataSize = 0;
    memcpy(&dataSize, packet.data(), 4);
    //std::cout << "Packet Size: " << dataSize << std::endl;

    if (dataSize > packet_body_length || dataSize > packet.size())
    {
        //big problem, packet too big
        *serverPtr->engine->console << "Malformed Packet" << std::endl;
        return;
    }

    memcpy(packet_body, (const char*)packet.data() + 4, dataSize);

    try
    {
        nlohmann::json j = nlohmann::json::parse(std::string(packet_body, dataSize));

        if (j["type"] == "ping")
        {
            if (j.contains("ts"))
            {
                uint64_t ts_ = j["ts"];
                uint64_t ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch()
                    ).count();
                *serverPtr->engine->console << "Client latency: " << (ms - ts_) / 2 << " ms" << std::endl;
            }
        }

        if (j["type"] == "cping") //client ping request
        {
            do_write(boost::asio::buffer(packet_body, dataSize));
        }

        if (j["type"] == "echo")
        {
            std::string data;
            macaron::Base64::Decode(j["data"], data);
            
            *serverPtr->engine->console << "echo: [" << data << "] "<< std::endl;

            do_write(boost::asio::buffer(packet_body, dataSize));
        }

        if (j["type"] == "clientUpdate")
        {
            entityClientCam->pos = Util::vec3FromString(j["data"]["pos"]);
            //"data": {  "pos": vec3, "ang" : vec3 }
        }
    }
    catch (nlohmann::json::exception& e)
    {
        std::cout << e.what() << std::endl;
    }
}

void session::do_read()
{
	std::lock_guard<std::mutex> lock(sessionLock);

    auto self(shared_from_this());
    socket_.async_read_some(boost::asio::buffer(data_, max_length),
        [this, self](boost::system::error_code ec, std::size_t length)
        {
            if (!ec)
            {
                readPacket(boost::asio::buffer(data_, length));
                do_read();
            }
            else {
                std::cout << socket_.remote_endpoint().address().to_string() << ": " << ec.message() << " [" << ec.value() << "]" << std::endl;
                if ((boost::asio::error::eof == ec) || (boost::asio::error::connection_reset == ec))
                {
                    disconnect();
                }
            }
        });
}

void session::sendClientPing()
{
    uint64_t ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
        ).count();
    nlohmann::json ts;
    ts["type"] = "ping";
    ts["ts"] = ms;
    std::string pingcmd = ts.dump();
    do_write(boost::asio::buffer(pingcmd.c_str(), pingcmd.size()));
}

void session::do_write(boost::asio::const_buffer response)
{
	const std::lock_guard<std::mutex> lock(writeLock);

    uint32_t totalSize = response.size() + 4;
    char* packet = new char[totalSize];

    uint32_t size = response.size();
    memcpy(&packet[0], &size, 4);
    memcpy(&packet[4], response.data(), size);

    auto self(shared_from_this());
    boost::asio::async_write(socket_, boost::asio::buffer(packet, totalSize),
        [this, self, packet](boost::system::error_code ec, std::size_t /*length*/)
        {
            if (!ec)
            {
            }
            delete[] packet;
        });
}

void session::notice(std::string notice)
{
    send({ {"type", "notice"}, {"data", macaron::Base64::Encode(notice)} });
}

void session::send(nlohmann::json message)
{
    std::string messageComposed = message.dump();
    do_write(boost::asio::buffer(messageComposed.c_str(), messageComposed.size()));
}

void session::disconnect()
{
    auto it = std::find(serverPtr->sessions.begin(), serverPtr->sessions.end(), shared_from_this());
    serverPtr->sessions.erase(it);

    const std::lock_guard<std::mutex> lock(serverPtr->engine->engineLock);
    if(entityClientCam != nullptr)
        entityClientCam->remove();   
    serverPtr->engine->entityManger->removeEntity(entityClientCam);
}

void session::printMessage(std::string message)
{
    std::string name;
    if (!username.empty())
        name = username;
    else
        name = socket_.remote_endpoint().address().to_string();

    std::cout << "[" << name << "]: " << message << std::endl;
}

void session::kick()
{
    send({ {"type", "notice"}, {"data", macaron::Base64::Encode("You have been kicked from the server.")}});
    disconnect();
    socket_.close();
}

void session::sendPeers()
{
    nlohmann::json j;
    j["type"] = "clients";
    
    for (auto& i : serverPtr->sessions)
    {
        if (i != shared_from_this())
        {
            j["data"].push_back(
                {
                    {"ip", i->getIP()},
                    {"pos", Util::printVec3(entityClientCam->pos)}
                });
        }
    }
    j["data"].push_back(
    {
        {"ip", "0.0.0.0"},
        {"pos", Util::printVec3(serverPtr->engine->camera->pos)}
    });

    send(j);
}

std::string session::getUsernameB64()
{
    return macaron::Base64::Encode(getUsername());
}

std::string session::getUsername()
{
    if (username.empty())
    {
        printMessage("Tried to get username of client but it's not set.");
    }
    return username;
}

void session::setUsername(std::string username_)
{
    username = username_;
}

std::string session::getIP()
{
    return socket_.remote_endpoint().address().to_string();
}
