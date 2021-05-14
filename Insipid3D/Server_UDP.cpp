#include "Server_UDP.h"
#include "engine.h"
#include <iostream>

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
        std::cout << "[" << remote_endpoint_.address().to_string() << "]: " << recv_buffer_.data() << std::endl;

        boost::shared_ptr<std::string> message(
            new std::string("test"));

        socket_.async_send_to(boost::asio::buffer(*message), remote_endpoint_,
            boost::bind(&Server_UDP::handle_send, this, message,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));

        start_receive();
    }
    if ((boost::asio::error::eof == error) || (boost::asio::error::connection_reset == error))
    {
        std::cout << "Client disconected: " << remote_endpoint_.address().to_string() << std::endl;
    }
}

void Server_UDP::handle_send(boost::shared_ptr<std::string>, const boost::system::error_code&, std::size_t)
{
}