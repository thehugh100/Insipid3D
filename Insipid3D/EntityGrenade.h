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
	virtual void explode();
	float explosionTime;
	int set;
};