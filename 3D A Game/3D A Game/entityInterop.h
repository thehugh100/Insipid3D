#pragma once
#include "entityManager.h"
#include <string>
#include <string.h>
#include "log.h"
#include <vector>
#include "HUtils.h"
#include "model.h"
#include "collider.h"
#include "entityWorld.h"
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
			if (i >= afterIndex && findEntity(i)->getClass().compare(class_) == 0)
				return findEntity(i);
		}
		return nullptr;
	}
	/*Returns a pointer to a list of entities of a certain class (must be freed)*/
	CEntityList *findEntitiesByClass(string class_)
	{
		CEntityList *list = new CEntityList;
		for (auto i : entityManager->entityList)
		{
			if (i->getClass().compare(class_) == 0)
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
		HUtils::XYZ e1Pos = e1->pos;
		HUtils::XYZ e2Pos = e2->pos;

		if (e1->getClass() == "entPlayer" || e1->getClass() == "entMine")
			e1Pos += HUtils::XYZ(0, 1.0f, 0); //player height

		if (e2->getClass() == "entPlayer" || e2->getClass() == "entMine")
			e2Pos += HUtils::XYZ(0, 1.0f, 0);

		return HUtils::distance2XYZ(e1Pos, e2Pos);
	}
	HUtils::XYZ entityDirection(CEntity *e1, CEntity *e2)
	{
		return (e1->pos - e2->pos).normalized();
	}
	/*Tests whether entity 1 can see entity 2*/
	bool entityVisible(CEntity *e1, CEntity *e2)
	{
		return 1;
		float distance = entityDistance(e1, e2);
		
		
		trace occluded = collider->findCollision(e1->pos, entityDirection(e1, e2), dynamic_cast<entWorld*>(getWorld())->mesh);

		if (occluded.didHit && !occluded.traceFailed && occluded.distance >= distance - 0.2) //0.2 is some leeway incase the two values aren't exactly the sames
		{
			return 1;
		}
		else
		{
			return 0;
		}
		return 0;
	}
};
CEntityInterop *entityInterop;