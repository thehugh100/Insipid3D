#pragma once
#include "Entity.h"
#include "Light.h"
#include "EntityPoint.h"
#include <glm/glm.hpp>
#include "Mesh.h"

struct EntityLight : EntityPoint
{
	EntityLight(Light *light);

	virtual void tick();
	virtual void render();
	virtual void init();
	virtual void remove();

	Light *light;
	Mesh* editorMesh;
};