#pragma once

#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>

struct Engine;

class Server_UDP
{
public:
    Server_UDP(boost::asio::io_service& io_service, int port, Engine* engine);

private:
    void start_receive();

    void handle_receive(const boost::system::error_code& error,
        std::size_t bytes);

    void handle_send(boost::shared_ptr<std::string> /*message*/,
        const boost::system::error_code& /*error*/,
        std::size_t /*bytes_transferred*/);

    boost::asio::ip::udp::socket socket_;
    boost::asio::ip::udp::endpoint remote_endpoint_;
    boost::array<char, 65536> recv_buffer_;

    Engine* engine;
};