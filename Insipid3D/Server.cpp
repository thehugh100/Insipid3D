#include "server.h"
#include "session.h"
#include <boost/filesystem.hpp>
#include "engine.h"

server::server(boost::asio::io_context& io_context, short port, Engine* engine)
    : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)), engine(engine)
{
    *engine->console << "Server Started on port: " << port << std::endl;

    do_accept();
}

void server::getOnlineUsers(nlohmann::json& online)
{
    online["type"] = "online";
    for (auto& i : sessions)
    {
        online["users"].push_back(i->getUsernameB64());
    }
}

void server::notice(std::string notice)
{
    std::cout << "Sending Notice: " << notice << std::endl;
    for (auto& i : sessions)
    {
        i->notice(notice);
    }
}

void server::do_accept()
{
    acceptor_.async_accept(
        [this](boost::system::error_code ec, tcp::socket socket)
        {
            if (!ec)
            {
                auto sessionPtr = std::make_shared<session>(std::move(socket), this);
                sessions.emplace_back(sessionPtr);
                sessionPtr->start();
            }

            do_accept();
        });
}
