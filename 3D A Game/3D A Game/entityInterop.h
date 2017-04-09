#pragma once
#include "entityManager.h"
#include <string>
#include <string.h>
#include "log.h"

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
};
CEntityInterop *entityInterop;