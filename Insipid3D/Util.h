#pragma once
#include <glm/glm.hpp>
#include <btBulletDynamicsCommon.h>

struct Util
{
	static glm::vec3 vec3Conv(btVector3 in);
	static btVector3 vec3Conv(glm::vec3 in);
	static float distanceSquared(glm::vec3 a, glm::vec3 b);
	static float randFloat();
	static glm::vec3 randVec();
};