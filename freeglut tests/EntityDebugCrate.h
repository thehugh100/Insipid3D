#pragma once
#include "entity.h"
#include "Mesh.h"
#include <glm/glm.hpp>

#include <btBulletDynamicsCommon.h>

struct EntityDebugCrate : Entity
{
	EntityDebugCrate(glm::vec3 pos);

	virtual void tick();
	virtual void render();
	virtual void init();

	glm::vec3 origin;

	Mesh* model;
	btRigidBody* body;
};
