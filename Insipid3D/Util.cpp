#include "Util.h"
#include <iostream>
#include <random>
#include <sstream>

glm::vec3 Util::vec3Conv(btVector3 in)
{
	return glm::vec3(in.getX(), in.getY(), in.getZ());
}

btVector3 Util::vec3Conv(glm::vec3 in)
{
	return btVector3(in.x, in.y, in.z);
}

float Util::distanceSquared(glm::vec3 a, glm::vec3 b)
{
	return ((b.x - a.x) * (b.x - a.x)) + ((b.y - a.y) * (b.y - a.y)) + ((b.z - a.z) * (b.z - a.z));
}

float Util::randFloat()
{
	return (float)rand() / RAND_MAX;
}

glm::vec3 Util::randVec()
{
	return glm::normalize(glm::vec3(randFloat() - .5f, randFloat() - .5f, randFloat() - .5f));
}

std::string Util::printVec3(glm::vec3 in)
{
	std::ostringstream o;
	o << in.x << ", " << in.y << ", " << in.z;
	return o.str();
}
