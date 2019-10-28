#include "EntityGrenade.h"
#include "EntityExplosion.h"
#include "engine.h"

EntityGrenade::EntityGrenade(glm::vec3 origin_)
{
	entityType = "EntityGrenade";
	entityTraits.setTrait("EntityGrenade");
	modelName = "models/editor/sphere.glb";
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
		}
		else
		{
			if (engine->getElapsedTimeMS() > explosionTime)
			{
				engine->entityManger->addEntity(new EntityExplosion(getPosition(), 4500.f));
				destroy();
			}
		}
	}
}

