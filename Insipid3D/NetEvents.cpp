#include "NetEvents.h"

NetEvents::NetEvents(Engine *engine)
	:engine(engine)
{

}

void NetEvents::pushInstruction(instruction_t instruction)
{
	instructions.push_back(instruction);
}

void NetEvents::tick()
{
	if (!instructions.empty())
	std::cout << "NetEvents::tick() - instructions.size() = " << instructions.size() << std::endl;

	for (auto& i : instructions)
		i();

	instructions.clear();
}
