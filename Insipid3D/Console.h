#pragma once
#include <vector>
#include <string>
#include <functional>
#include <map>

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

	std::map<std::string, std::function<std::string(std::string)>> commands;
	Engine* engine;
};