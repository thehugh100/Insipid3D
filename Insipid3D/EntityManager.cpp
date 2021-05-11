#include "EntityManager.h"
#include "engine.h"

void EntityManager::tick()
{
	size_t size = entities.size();
	for (int i = 0; i < size; ++i)
	{
		if(!entities[i]->initialised) //entites created through the network thread may not be initialised
			entities[i]->init();

		entities[i]->tick();
	}
}

void EntityManager::render()
{
	size_t size = entities.size();
	for (int i = 0; i < size; ++i)
	{
		entities[i]->render();
	}
}

Entity* EntityManager::addEntity(Entity* e)
{
	e->engine = engine;
	e->init();
	entities.push_back(e);
	return e;
}

Entity* EntityManager::addEntityNoInit(Entity* e)
{
	e->engine = engine;
	entities.push_back(e);
	return e;
}

EntityManager::EntityManager(Engine* enginePtr)
{
	engine = enginePtr;
}

bool EntityManager::getEntityByTraits(std::string trait, EntityList* entityList)
{
	if (entityList != nullptr)
	{
		int count = 0;
		for (auto& i : entities)
		{
			if (i->entityTraits.hasTrait(trait))
			{
				entityList->push_back(i);
				count++;
			}
		}
		if (count)
			return 1;
	}

	return 0;
}

void EntityManager::getAllEntities(EntityList* entityList)
{
	if (entityList != nullptr)
	{
		int count = 0;
		for (auto& i : entities)
		{

			entityList->push_back(i);
			count++;

		}
	}
}

void EntityManager::clear()
{
	size_t count = entities.size();
	for (int i = 0; i < count; ++i)
	{
		entities[i]->remove();
		delete entities[i];
	}

	entities.clear();
}
