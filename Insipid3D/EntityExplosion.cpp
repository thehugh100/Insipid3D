#include "EntityExplosion.h"
#include "engine.h"
#include "Util.h"

EntityExplosion::EntityExplosion(glm::vec3 origin_, float magnitude)
	:magnitude(magnitude)
{
	entityType = "EntityExplosion";
	entityTraits.setTrait("EntityExplosion");
	origin = origin_;
	point = &origin;
}

void EntityExplosion::tick()
{
	if (active)
	{
		EntityList e;
		engine->entityManger->getEntityByTraits("EntityPhysicsProp", &e);
		for (auto& i : e)
		{
			EntityPhysicsProp* ie = (EntityPhysicsProp*)i;
			if (ie->active)
			{
				float distance = glm::distance(origin, Util::vec3Conv(ie->body->getWorldTransform().getOrigin()));
				glm::vec3 direction = -glm::normalize(origin - Util::vec3Conv(ie->body->getWorldTransform().getOrigin()));

				float explosionStrength = 0;

				if (distance != 0)
					explosionStrength = magnitude / (distance * distance);

				btVector3 start = Util::vec3Conv(origin);
				btVector3 end = ie->body->getWorldTransform().getOrigin();

				btDynamicsWorld::ClosestRayResultCallback r(start, end);
				engine->getMap()->collisionState->world->rayTest(start, end, r);

				if (r.hasHit())
				{
					if (r.m_collisionObject->getWorldTransform().getOrigin() != btVector3(0, 0, 0))
					{
						ie->applyImpulse(direction * explosionStrength);
					}
				}
			}
		}
		remove();
	}
}

void EntityExplosion::render()
{
}

void EntityExplosion::init()
{
}

void EntityExplosion::remove()
{
	active = 0;
}
