#include "Util.h"

glm::vec3 Util::vec3Conv(btVector3 in)
{
	return glm::vec3(in.getX(), in.getY(), in.getZ());
}

btVector3 Util::vec3Conv(glm::vec3 in)
{
	return btVector3(in.x, in.y, in.z);
}
