#include "EntityGrenade.h"
#include "EntityExplosion.h"
#include "engine.h"
#include "Util.h"

#include <glm/gtc/quaternion.hpp>
#include <glm/common.hpp>

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
			body->setCcdMotionThreshold(0.1f);
			body->setCcdSweptSphereRadius(0.4f);
			body->setAngularVelocity(Util::vec3Conv(Util::randVec() * 5.f));

		}
		else
		{
			/*auto rot = body->getWorldTransform().getRotation();
			glm::quat gQuat = glm::quat(rot.getW(), rot.getX(), rot.getY(), rot.getZ());
			body->applyForce(Util::vec3Conv(gQuat * glm::vec3(0, mass * 60 * 2, 0)), btVector3(0, 1, 0));*/

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

