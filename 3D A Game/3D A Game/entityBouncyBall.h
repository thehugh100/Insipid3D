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
struct entBounceyBall : CEntity
{
	entBounceyBall(HUtils::XYZ pos_, HUtils::XYZ vel_)
	{
		pos = pos_;
		vel = vel_;
		active = 1;
		timer = std::chrono::system_clock::now();
	}
	entBounceyBall()
	{

	}
	virtual void tick()
	{
		if (active)
		{
			std::chrono::duration<double, std::milli> q = std::chrono::system_clock::now() - timer;
			if (q.count() > 1000)
				active = 0;
			pos += vel * globals->timeDelta;
			vel += HUtils::XYZ(0, -0.00008, 0);

			/*entityInterop->findEntity(1)->pos.print();*/

			entWorld *world = dynamic_cast<entWorld*>(entityInterop->getWorld());
			if (world != nullptr)
			{
				trace qpr = rayTrace(pos, vel, world->mesh);
				if (qpr.didHit && qpr.distance < 1 * max(1, vel.magnitude()))
				{
					vel += qpr.hitNormal * vel.magnitude() * 0.8;
				}
			}
			else
			{
				cout << "World not found\n";
			}
		}
	}
	virtual void render()
	{
		if (active)
		{
			glColor4f(1, 1, 1, 0.4f);
			glPushMatrix();
			glTranslatef(pos.x, pos.y, pos.z);
			glutSolidSphere(0.1, 5, 5);
			glPopMatrix();
		}
	}
	virtual std::string getClass()
	{
		return "entBouncyBall";
	}
	HUtils::XYZ vel;
	int active;
	std::chrono::system_clock::time_point timer;
};