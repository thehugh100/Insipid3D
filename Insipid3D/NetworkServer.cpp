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

    if (updateTimer > 1.f / 64.f) //64 ticks a second
    {
        serverTicks++;
        updateTimer = 0;
        if (active)
        {

            //i->send({ {"type", "tick"}, {"data", serverTicks} });
                
            EntityList l;
            engine->entityManger->getAllEntities(&l);
            //std::cout << l.size() << std::endl;
            nlohmann::json j;

            for (auto& i : l)
            {
                if(i->active && i->entityType == "EntityExplosiveBarrel")
                    j.push_back(i->serialize());
            }

            server_udp_->sendAllJson({ {"type", "entityUpdate"}, {"data", j}, {"tick", serverTicks} });
        }
    }
}
