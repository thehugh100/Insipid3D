#pragma once

#include <iostream>
#include <windows.h>
#include "GL\freeglut.h"
#include "globals.h"
#include "HUtils.h"
#include "entity.h"
#include <vector>

//using namespace HUtils;

/*Manages Updating and finding all entities*/
struct CEntityManager
{
	CEntityManager()
	{
		entityList.reserve(maxEntities);
	}
	/*Calls the tick() function for all registered entities.*/
	void tick()
	{
		for(auto i: entityList)
			i->tick();
	}
	/*Called from the render thread, renders all entities.*/
	void render()
	{
		for (auto i : entityList)
			i->render();
	}
	/*Add an entity and return its index*/
	CEntity *addEntity(CEntity *ent)
	{
		if (entityList.size() - 1 == maxEntities)
		{
			MessageBoxA(0, "Sorry, we've run out of entites for you, we hope to get some more at some point.", "Sorry, dave", 0);
		}
		else
		{
			entityList.push_back(ent);
			return ent;
		}
		return nullptr;
	}
	vector<CEntity*> entityList;
	int maxEntities = 9999;
};
CEntityManager *entityManager;