#pragma once
#include "entity.h"
#include "EntityPoint.h"
#include "Mesh.h"
#include <glm/glm.hpp>
#include "EntityPhysicsProp.h"
#include <btBulletDynamicsCommon.h>

struct EntityExplosion : EntityPoint
{
	EntityExplosion(glm::vec3 origin, float magnitude);
	virtual void tick();
	virtual void render();
	virtual void init();
	virtual void remove();
	float explosionTime;
	float magnitude;
	float clearTime;
	int set;
	glm::vec3 origin;
	glm::vec3 rotation;
	GLuint noiseTex;
};