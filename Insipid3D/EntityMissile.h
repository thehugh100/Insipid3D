#pragma once
#include "entity.h"
#include "Mesh.h"
#include <glm/glm.hpp>
#include "EntityPhysicsProp.h"
#include <btBulletDynamicsCommon.h>

struct EntityLight;
struct EntityFlare;

struct EntityMissile : EntityPhysicsProp
{
	EntityMissile(glm::vec3 origin);
	EntityMissile();

	virtual void tick();
	virtual void destroy();

	glm::vec3 oldVelocity;
	glm::vec3 velocity;
	int triggered;
	float explosionTime;
	bool set;
	int stage;
	float nextStage;
	EntityLight* cLight;
	EntityFlare* flare;
};
