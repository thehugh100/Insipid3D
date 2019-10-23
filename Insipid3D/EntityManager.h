#pragma once
#include "Entity.h"
#include <vector>
#include <string>

struct Engine;

typedef std::vector<Entity*> EntityList;

class EntityManager
{
public:
	EntityManager(Engine* enginePtr);
	void tick();
	void render();
	Entity* addEntity(Entity* e);
	bool getEntityByTraits(std::string trait, EntityList* entityList);
	void getAllEntities(EntityList* entityList);
private:
	Engine* engine;
	std::vector<Entity *> entities;
};