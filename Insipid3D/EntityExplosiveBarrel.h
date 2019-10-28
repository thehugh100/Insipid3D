#pragma once
#include "entity.h"
#include "Mesh.h"
#include <glm/glm.hpp>
#include "EntityPhysicsProp.h"
#include <btBulletDynamicsCommon.h>

struct EntityExplosiveBarrel : EntityPhysicsProp
{
	EntityExplosiveBarrel(glm::vec3 origin);
	EntityExplosiveBarrel();

	virtual void tick();
	virtual void destroy();

	glm::vec3 oldVelocity;
	glm::vec3 velocity;
	int triggered;
	float explosionTime;
}; 
