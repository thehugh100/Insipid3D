#pragma once

#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>

class Server_UDP;

class Session_UDP
{
public:
    Session_UDP(boost::asio::ip::udp::endpoint endpoint, boost::asio::ip::udp::socket *socket, Server_UDP* server_udp);
    void receiveData(std::string data);
private:
    boost::asio::ip::udp::endpoint endpoint;
    boost::asio::ip::udp::socket *socket;
    Server_UDP* server_udp;
};
