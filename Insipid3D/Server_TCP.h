#pragma once

#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <boost/asio.hpp>
#include <vector>
#include <mutex>
#include <nlohmann/json.hpp>

using boost::asio::ip::tcp;

class session_tcp;
struct Engine;

class server_tcp
{
public:
    server_tcp(boost::asio::io_context& io_context, short port, Engine* engine);

    void getOnlineUsers(nlohmann::json& online);
    void notice(std::string notice);
    std::vector<std::shared_ptr<session_tcp>> sessions;
    Engine* engine;

private:
    void do_accept();
    tcp::acceptor acceptor_;
};