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

private:
	std::vector<std::string> traits;
};