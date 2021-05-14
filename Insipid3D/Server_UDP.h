#pragma once

#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>

#include <map>
#include <nlohmann/json.hpp>

struct Engine;
class Session_UDP;

class Server_UDP
{
public:
    Server_UDP(boost::asio::io_service& io_service, int port, Engine* engine);
    std::map<std::string, Session_UDP*> sessions;

    void start_receive();

    void handle_receive(const boost::system::error_code& error,
        std::size_t bytes);

    void handle_send(const boost::system::error_code& /*error*/,
        std::size_t /*bytes_transferred*/);

    void sendAll(std::string data);
    void sendAllJson(nlohmann::json jsonData);
    void sendPeers();

    boost::asio::ip::udp::socket socket_;
    boost::asio::ip::udp::endpoint remote_endpoint_;
    boost::array<char, 65536> recv_buffer_;

    Engine* engine;
private:
};