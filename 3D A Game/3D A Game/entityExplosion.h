#pragma once
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

struct entExplosion : CEntity
{
	entExplosion(HUtils::XYZ pos_, bool affectPlayers_ = 1)
	{
		pos = pos_;
		timer = std::chrono::system_clock::now();
		affectPlayers = affectPlayers_;
		if (affectPlayers)
		{
			/*Make this more generic so it affects all phys objects*/
			for (auto i : entityInterop->findEntitiesByClass("entPlayer")->entities)
			{
				float magnitude = min(0.2f / entityInterop->entityDistance(this, i), 1.0f); //clamp maximum velocity
				HUtils::XYZ direction = (pos - i->pos).normalized();
				i->vel += direction * magnitude;
				i->pos += HUtils::XYZ(0, 0.1, 0); //kick player off ground so they don't get stuck
			}
		}
	}
	void tick()
	{
		if (active)
		{
			ticks++;
			size += 0.03f * globals->timeDelta;
			std::chrono::duration<double, std::milli> q = std::chrono::system_clock::now() - timer;
			if (q.count() > 200)
				active = 0;
			alpha = 1.0f - (q.count() / 200.0f);
		}
	}
	void render()
	{
		if (active)
		{
			glPushMatrix();
			glColor4f(0.8, 0.8, 0.1, alpha);
			glTranslatef(pos.x, pos.y, pos.z);
			glutSolidSphere(size, 100, 100);
			glPopMatrix();
		}
	}
	float size = 0;
	int active = 1;
	int ticks = 0;
	float alpha = 0;
	std::chrono::system_clock::time_point timer;
	bool affectPlayers = 0;
};
