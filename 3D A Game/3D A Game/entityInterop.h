#pragma once
#include "entityManager.h"
#include <string>
#include <string.h>
#include "log.h"
#include <vector>
#include "HUtils.h"
struct CEntityList
{
	std::vector<CEntity*> entities;
};

struct CEntityInterop
{
	CEntity *findEntity(int index)
	{
		return entityManager->entityList[index];
	}
	CEntity *findEntityByClass(string class_, int afterIndex = 0)
	{
		for (int i = 0; i < entityManager->entityList.size(); i++)
		{
			if (i >= afterIndex && findEntity(i)->getClass() == class_)
				return findEntity(i);
			return nullptr;
		}
	}
	/*Returns a pointer to a list of entities of a certain class (must be freed)*/
	CEntityList *findEntitiesByClass(string class_)
	{
		CEntityList *list = new CEntityList;
		for (auto i : entityManager->entityList)
		{
			if (i->getClass() == class_)
				list->entities.push_back(i);
		}
		return list;
	}
	CEntity *getWorld()
	{
		CEntity *p = findEntityByClass("entWorld");
		if (p == nullptr)
		{
			alert("World is nullptr in CEntityInterop::getWorld()");
		}
		return p;
	}
	int getEntityCount()
	{
		return entityManager->entityList.size();
	}
	float entityDistance(CEntity *e1, CEntity *e2)
	{
		return HUtils::distance2XYZ(e1->pos, e2->pos);
	}
};
CEntityInterop *entityInterop;