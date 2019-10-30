#include "EntityGrenade.h"
#include "EntityExplosion.h"
#include "engine.h"
#include "Util.h"

EntityGrenade::EntityGrenade(glm::vec3 origin_)
{
	entityType = "EntityGrenade";
	entityTraits.setTrait("EntityGrenade");
	modelName = "models/grenade.glb";
	mass = 4;
	origin = origin_;
	set = 0;
}

void EntityGrenade::tick()
{
	if (active)
	{
		if (!set)
		{
			set = 1;
			explosionTime = engine->getElapsedTimeMS() + 2000.0f;
			body->setRestitution(1.0f);
			body->setCcdMotionThreshold(0.2f);
			body->setCcdSweptSphereRadius(0.4f);
			body->setAngularVelocity(Util::vec3Conv(Util::randVec() * 5.f));
		}
		else
		{
			if (engine->getElapsedTimeMS() > explosionTime)
			{
				explode();
			}
		}
	}
}

void EntityGrenade::explode()
{
	engine->entityManger->addEntity(new EntityExplosion(getPosition(), 4500.f));
	destroy();
}

