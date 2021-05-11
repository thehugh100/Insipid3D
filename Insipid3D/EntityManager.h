#pragma once
#include "Entity.h"
#include <vector>
#include <string>
#include <mutex>

struct Engine;

typedef std::vector<Entity*> EntityList;

class EntityManager
{
public:
	EntityManager(Engine* enginePtr);
	void tick();
	void render();
	Entity* addEntity(Entity* e);
	Entity* addEntityNoInit(Entity* e);
	bool getEntityByTraits(std::string trait, EntityList* entityList);
	void getAllEntities(EntityList* entityList);
	void clear();
private:
	Engine* engine;
	std::vector<Entity *> entities;
};