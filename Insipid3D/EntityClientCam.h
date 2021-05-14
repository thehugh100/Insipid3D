#pragma once
#include "Entity.h"
#include "EntityPoint.h"
#include <glm/glm.hpp>
#include "Mesh.h"

struct EntityClientCam : Entity
{
	EntityClientCam(glm::vec3 pos);

	virtual void tick();
	virtual void render();
	virtual void init();
	virtual void remove();

	Mesh* editorMesh;
	glm::vec3 pos;
	glm::vec3 lookVec;
	glm::vec3 hitPos;
};