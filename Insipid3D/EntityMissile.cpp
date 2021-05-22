#include "EntityMissile.h"
#include "Util.h"
#include "engine.h"
#include "EntityExplosion.h"
#include "Light.h"
#include "EntityLight.h"
#include "EntityFlare.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/common.hpp>

EntityMissile::EntityMissile(glm::vec3 origin_)
{
	entityType = "EntityMissile";
	entityTraits.setTrait("EntityMissile");
	modelName = "models/missile.glb";
	origin = origin_;
	mass = 136;
	oldVelocity = glm::vec3(0.f);
	velocity = glm::vec3(0.f);
	triggered = 0;
	explosionTime = 0;
	set = 0;
	vars.registerVal("explosionTime", Serializer(&explosionTime));
	vars.registerVal("triggered", Serializer(&triggered));
	stage = 0;
	nextStage = 0;
}

EntityMissile::EntityMissile()
{
	entityType = "EntityMissile";
	entityTraits.setTrait("EntityMissile");
	modelName = "models/missile.glb";
	origin = glm::vec3(0.f);
	mass = 136;
	oldVelocity = glm::vec3(0.f);
	velocity = glm::vec3(0.f);
	triggered = 0;
	explosionTime = 0;
	set = 0;
	vars.registerVal("explosionTime", Serializer(&explosionTime));
	vars.registerVal("triggered", Serializer(&triggered));
	stage = 0;
	nextStage = 0;
}

void EntityMissile::tick()
{
	if (active)
	{
		if (!set)
		{
			body->setCcdMotionThreshold(.1f);
			body->setCcdSweptSphereRadius(1.f);
			set = 1;
			nextStage = engine->getElapsedTimeMS() + 100.0f;


			glm::vec3 targetDelta = engine->camera->lookVec;
			btTransform t = body->getWorldTransform();
			glm::quat lQuat = glm::quatLookAt(glm::normalize(targetDelta), glm::vec3(0, 1, 0)) * glm::quat(glm::vec3(-3.1415 / 2.f, 0, 0));

			t.setRotation(btQuaternion(lQuat.x, lQuat.y, lQuat.z, lQuat.w));
			body->setWorldTransform(t);

			body->applyCentralImpulse(Util::vec3Conv(engine->camera->lookVec) * mass * 10.f);

			cLight = engine->getMap()->addLight(new Light(getPosition(), glm::vec3(1., 0.95, 0.8), 500, 0, glm::vec3(0, -1, 0), 1));
			cLight->light->intensity = 2400.0f;

			flare = (EntityFlare*)engine->entityManger->addEntity(new EntityFlare(getPosition(), glm::vec3(1., 0.95, 0.8)));
		}

		update();

		oldVelocity = velocity;
		velocity = Util::vec3Conv(body->getLinearVelocity());

		float jolt = glm::abs(glm::length(oldVelocity) - glm::length(velocity));

		int beingGrabbed = engine->editor->selectedEntity == this && engine->input->mouseDown(GLFW_MOUSE_BUTTON_LEFT);

		if (jolt >= 25.0f && !beingGrabbed && !triggered) // force required to explode, don't explode if we're grabbing it
		{
			triggered = 1;
			engine->entityManger->addEntity(new EntityExplosion(getPosition(), 4500.f));
			destroy();
			return;
		}

		//apply force snippet
		auto rot = body->getWorldTransform().getRotation();
		glm::quat gQuat = glm::quat(rot.getW(), rot.getX(), rot.getY(), rot.getZ());

		//body->applyForce(Util::vec3Conv(gQuat * glm::vec3(0, mass * 10, 0)), btVector3(0.0001,2,0));
		//body->applyTorque(Util::vec3Conv(glm::vec3(0,mass,0)));

		if (engine->getElapsedTimeMS() > nextStage)
		{
			nextStage = engine->getElapsedTimeMS() + 100;
			//engine->entityManger->addEntity(new EntityExplosion(getPosition(), 0));
			stage++;
		}

		//if (stage == 0) //get high
		//{
		//	body->applyForce(Util::vec3Conv(gQuat * glm::vec3(0, mass * 65, 0)), btVector3(0., -1.5, 0));
		//}
		//else

		//glm::vec3 targetDelta = engine->camera->pos - Util::vec3Conv(body->getWorldTransform().getOrigin());
		//btTransform t = body->getWorldTransform();

		//btQuaternion origRot = t.getRotation();
		//glm::quat origRotQuat(origRot.getW(), origRot.getX(), origRot.getY(), origRot.getZ());

		//glm::quat lQuat = glm::quatLookAt(glm::normalize(targetDelta), glm::vec3(0, 1, 0)) * glm::quat(glm::vec3(-3.1415 / 2.f, 0, 0));

		//lQuat = glm::slerp<float>(origRotQuat, lQuat, 0.01);

		/*t.setRotation(btQuaternion(lQuat.x, lQuat.y, lQuat.z, lQuat.w));*/
		//body->setWorldTransform(t);

		//body->setGravity(btVector3(0, 0, 0));

		body->setGravity(btVector3(0, -12, 0));
		body->activate(true);

		body->applyTorque(body->getInvInertiaTensorWorld().inverse() * (body->getWorldTransform().getBasis() * btVector3(0, body->getLinearVelocity().length() * .5, 0)));

		body->applyForce(Util::vec3Conv(gQuat * glm::vec3(0, mass  * 80, 0)), btVector3(0., 0.0, 0));
		
		cLight->light->pos = getPosition() + (gQuat * glm::vec3(0, -1.1, 0));
		flare->pos = getPosition() + (gQuat * glm::vec3(0,-2.4,0));
	}
}

void EntityMissile::destroy()
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
	if (cLight != nullptr)
		cLight->active = 0;

	flare->remove();
	flare = nullptr;

	remove();
}
