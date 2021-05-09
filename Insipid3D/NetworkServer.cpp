#include "NetworkServer.h"
#include "engine.h"
#include "Server.h"
#include "Session.h"
#include "Util.h"
#include "camera.h";

#include <boost/filesystem.hpp>
#include <thread>

NetworkServer::NetworkServer(Engine* engine)
	:engine(engine)
{
    active = 0;
    port = 32500;
    server_ = nullptr;
}

void NetworkServer::startServer()
{
    *engine->console << "Trying to start server..." << std::endl;

    std::thread netThread([&] {
        try
        {
            server_ = new server(io_context, port, engine);
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

    if (updateTimer > 1.f / 60.f) //60 ticks a second
    {
        serverTicks++;
        updateTimer = 0;
        if (active)
        {
            for (auto& i : server_->sessions)
            {
                //i->send({ {"type", "tick"}, {"data", serverTicks} });
                
                /*EntityList l;
                engine->entityManger->getAllEntities(&l);

                nlohmann::json j;

                for (auto& i : l)
                    j.push_back(nlohmann::json::parse(i->serialize()));*/

                i->sendPeers();
            }
        }
    }
}
