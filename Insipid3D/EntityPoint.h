#pragma once
#include "Entity.h"
#include "Light.h"
#include <glm/glm.hpp>
#include "Mesh.h"

struct EntityPoint : Entity
{
	EntityPoint();

	virtual void tick() = 0;
	virtual void render() = 0;
	virtual void init() = 0;
	virtual void remove() = 0;

	void renderHandles();

	virtual glm::vec3 getPos();
	virtual void setPos(glm::vec3 in);

	glm::vec3* point;
};