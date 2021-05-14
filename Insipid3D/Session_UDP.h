#pragma once

#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <nlohmann/json.hpp>

class Server_UDP;

class Session_UDP
{
public:
    Session_UDP(boost::asio::ip::udp::endpoint endpoint, boost::asio::ip::udp::socket *socket, Server_UDP* server_udp);
    void receiveData(std::string data);
    void sendData(std::string data);
    void sendJson(nlohmann::json jsonData);

    boost::asio::ip::udp::endpoint endpoint;
    boost::asio::ip::udp::socket *socket;
    Server_UDP* server_udp;
private:
};
