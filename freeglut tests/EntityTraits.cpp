#include "EntityTraits.h"

void EntityTraits::setTrait(std::string trait)
{
	traits.push_back(trait);
}

bool EntityTraits::hasTrait(std::string trait)
{
	for (auto& i : traits)
		if (i.compare(trait) == 0) return true;

	return false;
}