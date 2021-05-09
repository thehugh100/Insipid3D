#pragma once

#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <boost/asio.hpp>
#include <vector>
#include <chrono>
#include <mutex>
#include <nlohmann/json.hpp>

#include "EntityClientCam.h"

using boost::asio::ip::tcp;

class server;

class session
    : public std::enable_shared_from_this<session>
{
public:
    session(tcp::socket socket, server* serverPtr);
    void start();
    void readPacket(boost::asio::const_buffer packet);
    void do_write(boost::asio::const_buffer response);

    void notice(std::string notice);
    void send(nlohmann::json message);
    void disconnect();
    void printMessage(std::string message);
    void kick();
    void sendPeers();
    std::string getUsernameB64();
    std::string getUsername();
    void setUsername(std::string username_);
    std::string getIP();
private:
    void do_read();
    void sendClientPing();

    std::string uid;
    std::string username;
	std::mutex sessionLock;
	std::mutex writeLock;

	std::chrono::high_resolution_clock::time_point lastBytesSentTS;
	size_t lastBytesSent;

    tcp::socket socket_;
    enum { max_length = 4096 };
    char data_[max_length];
    const int packet_body_length;
    char packet_body[4096];
    server *serverPtr;
    EntityClientCam* entityClientCam;
};