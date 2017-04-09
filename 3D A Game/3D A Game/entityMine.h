#pragma once
#include "entity.h"
#include "engine.h"
#include "HUtils.h"
#include "globals.h"
#include "entityManager.h"
#include "input.h"
#include <chrono>
#include "entityInterop.h"
#include "model.h"
#include "entityWorld.h"
#include "entityExplosion.h"
struct entMine : CEntity
{
	entMine()
	{

	}
	entMine(HUtils::XYZ _pos)
	{
		pos = _pos;
		mineSet = 0;
	}

	void explode()
	{
		active = 0;

		entExplosion *exp = new entExplosion(pos);
		entityManager->addEntity(exp);

		CEntityList* ents = entityInterop->findEntitiesByClass("entPlayer");
		for (auto i : ents->entities)
		{
			if (entityInterop->entityDistance(this, i) < 20)
			{

			}
		}
	}

	void tick()
	{
		if (active)
		{
			if (!mineSet)
			{
				trace ground = collider->findCollision(pos, HUtils::XYZ(0, -1, 0), dynamic_cast<entWorld*>(entityInterop->getWorld())->mesh);
				if (ground.didHit && !ground.traceFailed)
				{
					if (ground.distance < 0.3)
					{
						mineSet = 1;
						vel = HUtils::XYZ(0, 0, 0);
						pos = ground.hitPos + HUtils::XYZ(0, 0.06, 0);

						//bodyAngle.x = asin(ground.hitNormal.y) * 180 / 3.1415; //pitch
						//bodyAngle.z = atan2f(ground.hitNormal.z, ground.hitNormal.x) * 180 / 3.1415; //yaw
						HUtils::XYZ ds = ground.hitNormal;
						float i = ds.y;
						ds.y = ds.z;
						ds.z = i;

						bodyAngle.x = 90;
						//bodyAngle.z = atan2f(ds.x, ds.y) * 180 / 3.1415;
						//bodyAngle.x = atan2f(ds.z, sqrt((ds.x * ds.x) + (ds.y * ds.y))) * 180 / 3.1415;
					}
					else
					{
						vel.y -= globals->gravity * globals->timeDelta;
					}
				}
			}
			else
			{
				if (triggered)
				{
					vel.y -= globals->gravity * globals->timeDelta;
					std::chrono::duration<double, std::milli> q = std::chrono::system_clock::now() - timer;
					if (q.count() > 500)
					{
						explode();
					}
				}
				else
				{
					CEntityList* ents = entityInterop->findEntitiesByClass("entPlayer");
					for (auto i : ents->entities)
					{
						if (entityInterop->entityDistance(this, i) < 4)
						{
							triggered = 1;
							vel += HUtils::XYZ(0, 0.0057, 0);
							timer = std::chrono::system_clock::now();
							break;
						}
					}
					delete(ents);
				}
			}
			pos += vel/* * globals->timeDelta*/;
		}
	}
	void render()
	{
		if (active)
		{
			glPushMatrix();
			glColor4f(0.5, 0.5, 0.5, 1);
			glTranslatef(pos.x, pos.y, pos.z);
			glRotatef(bodyAngle.x, 1, 0, 0);
			glRotatef(bodyAngle.z, 0, 0, 1);
			glRotatef(bodyAngle.y, 0, 1, 0);
			glutSolidCylinder(0.4, 0.12, 20, 20);
			glPopMatrix();
		}
	}
	HUtils::XYZ vel;
	HUtils::XYZ bodyAngle;
	int mineSet = 0;
	int triggered = 0;
	int active = 1;
	std::chrono::system_clock::time_point timer;
};