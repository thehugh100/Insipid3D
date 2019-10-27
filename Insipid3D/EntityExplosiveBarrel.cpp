#include "EntityExplosiveBarrel.h"
#include "Util.h"
#include "engine.h"

EntityExplosiveBarrel::EntityExplosiveBarrel(glm::vec3 origin_)
{
	entityType = "EntityExplosiveBarrel";
	entityTraits.setTrait("EntityExplosiveBarrel");
	modelName = "models/barrel.glb";
	origin = origin_;

	oldVelocity = glm::vec3(0.f);
	velocity = glm::vec3(0.f);
}

void EntityExplosiveBarrel::tick()
{
	if (active)
	{
		oldVelocity = velocity;
		velocity = Util::vec3Conv(body->getLinearVelocity());

		float jolt = glm::abs(glm::length(oldVelocity) - glm::length(velocity));

		if (jolt >= 30.0f) // force required to explode
		{
			EntityList e;
			engine->entityManger->getEntityByTraits("EntityPhysicsProp", &e);

			for (auto& i : e)
			{
				EntityPhysicsProp* ie = (EntityPhysicsProp*)i;
				if (ie->active && ie != this)
				{
					float distance = glm::distance(Util::vec3Conv(body->getWorldTransform().getOrigin()), Util::vec3Conv(ie->body->getWorldTransform().getOrigin()));
					glm::vec3 direction = -glm::normalize(Util::vec3Conv(body->getWorldTransform().getOrigin()) - Util::vec3Conv(ie->body->getWorldTransform().getOrigin()));

					float explosionStrength = 0;

					if(distance != 0)
						explosionStrength = 4500.0f / (distance * distance);

					btVector3 start = ie->body->getWorldTransform().getOrigin();
					btVector3 end = body->getWorldTransform().getOrigin();

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

			destroy();
		}
	}
}

void EntityExplosiveBarrel::destroy()
{
	for (int i = 0; i < 4; i++)
	{
		/*EntityPhysicsProp* q = (EntityPhysicsProp*)engine->entityManger->addEntity(new EntityPhysicsProp("models/barrel_fragment.glb", getPosition()));
		q->backfaceCull = 0;

		btTransform a = q->body->getWorldTransform();
		float ang = glm::radians((i * 90.f));
		btQuaternion rot = a.getRotation();
		rot.setRotation(btVector3(0, 1, 0), ang);
		a.setRotation(rot);
		q->body->setWorldTransform(a);
		
		btVector3 gibLoc = q->body->getWorldTransform().getOrigin() + q->body->getCenterOfMassTransform().getOrigin();
		btTransform tt;
		body->getMotionState()->getWorldTransform(tt);
		//q->applyImpulse(  (gibLoc - tt.getOrigin()) );
		*/
	}

	remove();
}
