#include "NetworkServer.h"
#include "engine.h"
#include "Server_TCP.h"
#include "Session_TCP.h"
#include "Server_UDP.h"
#include "Session_UDP.h"

#include "Util.h"
#include "camera.h";

#include <boost/filesystem.hpp>
#include <thread>
#include <sstream>

NetworkServer::NetworkServer(Engine* engine)
	:engine(engine)
{
    active = 0;
    port = 32500;
    server_tcp_ = nullptr;
}

void NetworkServer::startServer()
{
    *engine->console << "Trying to start server..." << std::endl;

    std::thread netThread([&] {
        try
        {
            /*server_tcp_ = new server_tcp(io_context, port, engine);
            active = 1;
            io_context.run();
            active = 0;
            *engine->console << "Server Stopped." << std::endl;*/

            server_udp_ = new Server_UDP(io_context, port, engine);
            active = 1;
            io_context.run();
            active = 0;
            *engine->console << "Server Stopped." << std::endl;
        }
        catch (std::exception& e)
        {
            //std::cerr << "Exception: " << e.what() << "\n";
            *engine->console << "Server Exception: " << e.what() << std::endl;
        }
        active = 0;
    });
    
    netThread.detach();
}

void NetworkServer::stopServer()
{
    io_context.stop();
}

void NetworkServer::tick(float deltaTime)
{
    updateTimer += deltaTime;

    if (updateTimer > 1.f / 16.f) //16 ticks a second
    {
        serverTicks++;
        updateTimer = 0;
        if (active)
        {
            std::ostringstream msg;
            msg << "tick: " << serverTicks;

            boost::shared_ptr<std::string> message(
                new std::string(msg.str().data()));


            for (auto& i : server_udp_->sessions)
            {
                server_udp_->socket_.async_send_to(boost::asio::buffer(*message), i.second->endpoint,
                    boost::bind(&Server_UDP::handle_send, server_udp_, message,
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
            }

            //for (auto& i : server_tcp_->sessions)
            //{
            //    //i->send({ {"type", "tick"}, {"data", serverTicks} });
            //    
            //    EntityList l;
            //    engine->entityManger->getAllEntities(&l);

            //    nlohmann::json j;

            //    for (auto& i : l)
            //    {
            //        if(i->active)
            //            j.push_back(nlohmann::json::parse(i->serialize()));
            //    }

            //    i->send({ {"type", "entityUpdate"}, {"data", j} });
            //    std::cout << "sent " << serverTicks << std::endl;
            //    //i->sendPeers();
            //}
        }
    }
}
