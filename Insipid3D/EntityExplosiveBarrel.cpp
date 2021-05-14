#include "EntityExplosiveBarrel.h"
#include "Util.h"
#include "engine.h"
#include "EntityExplosion.h"

#include <glm/gtc/quaternion.hpp>
#include <glm/common.hpp>

EntityExplosiveBarrel::EntityExplosiveBarrel(glm::vec3 origin_)
{
	entityType = "EntityExplosiveBarrel";
	entityTraits.setTrait("EntityExplosiveBarrel");
	modelName = "models/barrel.glb";
	origin = origin_;
	mass = 136;
	oldVelocity = glm::vec3(0.f);
	velocity = glm::vec3(0.f);
	triggered = 0;
	explosionTime = 0;
	vars.registerVal("explosionTime", Serializer(&explosionTime));
	vars.registerVal("triggered", Serializer(&triggered));
}

void EntityExplosiveBarrel::tick()
{
	if (active)
	{
		update();
		oldVelocity = velocity;
		velocity = Util::vec3Conv(body->getLinearVelocity());

		float jolt = glm::abs(glm::length(oldVelocity) - glm::length(velocity));

		int beingGrabbed = engine->editor->selectedEntity == this && engine->input->mouseDown(GLFW_MOUSE_BUTTON_LEFT);

		if (jolt >= 30.0f && !beingGrabbed && !triggered) // force required to explode, don't explode if we're grabbing it
		{
			triggered = 1;
			explosionTime = engine->getElapsedTimeMS() + Util::randFloat() * 1000.0f;
		}
		if (triggered && engine->getElapsedTimeMS() > explosionTime)
		{
			engine->entityManger->addEntity(new EntityExplosion(getPosition(), 4500.f));
			destroy();
		}
		
		//apply force snippet
		/*auto rot = body->getWorldTransform().getRotation();
		glm::quat gQuat = glm::quat(rot.getW(), rot.getX(), rot.getY(), rot.getZ());

		body->applyForce(Util::vec3Conv(gQuat * glm::vec3(0, mass * 65, 0)), btVector3(0.001,1,0));*/
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
