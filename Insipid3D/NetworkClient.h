#pragma once
#include <iostream>

struct Engine;

class NetworkClient
{
public:
	NetworkClient(Engine* engine);
private:
	Engine* engine;
};