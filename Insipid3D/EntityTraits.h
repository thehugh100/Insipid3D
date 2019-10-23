#pragma once
#include <iostream>
#include <string>
#include <string.h>
#include <vector>

class EntityTraits
{
public:
	void setTrait(std::string trait);
	bool hasTrait(std::string trait);
	typedef std::vector<std::string> EntityTraits_t;

	EntityTraits_t* getTraits();
private:
	EntityTraits_t traits;
};