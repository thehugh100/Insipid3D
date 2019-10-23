#pragma once
#include <vector>
#include <string>
#include <functional>
#include <map>
#include <iostream>
#include <sstream>

struct Engine;

struct Console
{
	Console(Engine* enginePtr);

	void tick();
	void render();
	void consolePrint(std::string message);
	void parseCommand(std::string command);
	bool consoleShowing;
	int commandHistoryIndex;
	std::vector<std::string> consoleBuffer;
	std::vector<std::string> commandHistory;
	std::string preAutocompleteInputBuf;
	int lastAutocompleteIndex;
	int consoleScroll;
	int consoleLines;
	std::map<std::string, std::function<std::string(std::string)>> commands;
	Engine* engine;

	std::stringstream conSSBuf;
	template <class T>
	Console& operator<< (T const& in)
	{
		conSSBuf << in;
		return *this;
	}
	Console& operator<< (std::ostream& (*f)(std::ostream&))
	{
		/* This is a bit much but it works... */
		if (f == (std::basic_ostream<char> & (*)(std::basic_ostream<char>&)) & std::endl)
		{
			consolePrint(conSSBuf.str());
			conSSBuf.clear();
			conSSBuf.str("");
		}
		return *this;
	}
};