#pragma once
/*Some engine helper functions for logging and messaging*/

#include <iostream>
#include <Windows.h>
#include <string.h>
#include <string>

void alert(std::string message)
{
	std::cout << message << std::endl;
	//MessageBoxA(0, message.c_str(), "Engine Message", 0);
}