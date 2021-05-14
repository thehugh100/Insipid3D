#include "Server_UDP.h"
#include "engine.h"
#include <iostream>
#include "Session_UDP.h"

Server_UDP::Server_UDP(boost::asio::io_service& io_service, int port, Engine* engine)
    : socket_(io_service, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port)), engine(engine)
{
    *engine->console << "Server Started on port: " << port << std::endl;
    start_receive();
}

void Server_UDP::start_receive()
{
    socket_.async_receive_from(
        boost::asio::buffer(recv_buffer_), remote_endpoint_,
        boost::bind(&Server_UDP::handle_receive, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
}

void Server_UDP::handle_receive(const boost::system::error_code& error, std::size_t bytes)
{
    if (!error || error == boost::asio::error::message_size)
    {
        //*engine->console << "[" << remote_endpoint_.address().to_string() << ":" << remote_endpoint_.port() << "]: " << recv_buffer_.data() << std::endl;

        std::string sessionString = remote_endpoint_.address().to_string() + ":" + std::to_string(remote_endpoint_.port());

        if (sessions.find(sessionString) == sessions.end()) // new session
        {
            sessions[sessionString] = new Session_UDP(remote_endpoint_, &socket_, this);
        }

        sessions[sessionString]->receiveData(recv_buffer_.data());

        /*boost::shared_ptr<std::string> message(
            new std::string("test"));

        socket_.async_send_to(boost::asio::buffer(*message), remote_endpoint_,
            boost::bind(&Server_UDP::handle_send, this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));*/

        

        start_receive();
    }
}

void Server_UDP::handle_send(const boost::system::error_code&, std::size_t)
{
}

void Server_UDP::sendAll(std::string data)
{
    for (auto& i : sessions)
    {
        i.second->sendData(data);
    }
}

void Server_UDP::sendAllJson(nlohmann::json jsonData)
{
    for (auto& i : sessions)
    {
        i.second->sendJson(jsonData);
    }
}
