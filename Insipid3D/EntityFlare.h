#pragma once
#include "entity.h"
#include "Mesh.h"
#include <glm/glm.hpp>
#include "EntityPhysicsProp.h"
#include <btBulletDynamicsCommon.h>

struct EntityFlare : Entity
{
	EntityFlare(glm::vec3 origin, glm::vec3 col);
	EntityFlare();

	virtual void tick();
	virtual void render();
	virtual void init();
	virtual void remove();

	int flareTex;
	glm::vec3 pos;
	glm::vec3 col;
};
