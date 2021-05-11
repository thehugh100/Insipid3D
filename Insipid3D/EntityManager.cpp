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
	currentIndex++;
	e->engine = engine;
	e->id = currentIndex;
	e->init();
	entities.push_back(e);
	return e;
}

Entity* EntityManager::addEntityNoInit(Entity* e)
{
	currentIndex++;
	e->engine = engine;
	e->id = currentIndex;
	entities.push_back(e);
	return e;
}

EntityManager::EntityManager(Engine* enginePtr)
{
	engine = enginePtr;
	currentIndex = 0;
}

Entity* EntityManager::getEntityByID(size_t id)
{
	for (auto& i : entities)
		if (i->id == id)
			return i;

	return nullptr;
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

void EntityManager::removeEntity(Entity* e)
{
	auto it = std::find(entities.begin(), entities.end(), e);
	entities.erase(it);
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
