#pragma once
#include "entity.h"
#include "Mesh.h"
#include <glm/glm.hpp>
#include "EntityPhysicsProp.h"
#include <btBulletDynamicsCommon.h>

struct EntityGrenade : EntityPhysicsProp
{
	EntityGrenade(glm::vec3 origin);
	virtual void tick();
	float explosionTime;
	int set;
};