#include "NetEvents.h"

NetEvents::NetEvents(Engine *engine)
	:engine(engine)
{

}

void NetEvents::pushInstruction(instruction_t instruction)
{
	const std::lock_guard<std::mutex> lock(netEventLock);

	instructions.push_back(instruction);
}

void NetEvents::tick()
{
	const std::lock_guard<std::mutex> lock(netEventLock);

	if (!instructions.empty())
	std::cout << "NetEvents::tick() - instructions.size() = " << instructions.size() << std::endl;

	for (auto& i : instructions)
		i();

	instructions.clear();
}
