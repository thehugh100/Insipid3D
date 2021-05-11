#pragma once
#include <iostream>
#include <functional>
#include <vector>

typedef std::function<void()> instruction_t;

struct Engine;

class NetEvents
{
public:
	NetEvents(Engine* engine);
	void pushInstruction(instruction_t instruction);
	void tick();

private:
	std::vector<instruction_t> instructions;
	Engine* engine;
};