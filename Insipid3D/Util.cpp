#include "Util.h"
#include <random>

glm::vec3 Util::vec3Conv(btVector3 in)
{
	return glm::vec3(in.getX(), in.getY(), in.getZ());
}

btVector3 Util::vec3Conv(glm::vec3 in)
{
	return btVector3(in.x, in.y, in.z);
}

float Util::randFloat()
{
	return (float)rand() / RAND_MAX;
}

glm::vec3 Util::randVec()
{
	return glm::normalize(glm::vec3(randFloat() - .5f, randFloat() - .5f, randFloat() - .5f));
}
