#include "Editor.h"
#include "engine.h"
#include "Entity.h"
#include "EntityPhysicsProp.h"
#include "Util.h"

#include <glm/glm.hpp>

Editor::Editor(Engine *enginePtr)
{
	engine = enginePtr;
}

void Editor::tick()
{
	
	if (engine->input->mousePressed(GLFW_MOUSE_BUTTON_LEFT))
	{
		auto world = engine->getMap()->collisionState->world;

		btVector3 start = Util::vec3Conv(engine->camera->pos);
		btVector3 end = Util::vec3Conv(engine->camera->pos + engine->camera->lookVec * 200.0f);

		btDynamicsWorld::ClosestRayResultCallback r(start, end);
		world->rayTest(start, end, r);

		if (r.hasHit())
		{
			btVector3 xyz = r.m_collisionObject->getWorldTransform().getOrigin();

			bool foundGoodObject = 0;

			EntityList e;
			engine->entityManger->getEntityByTraits("EntityPhysicsProp", &e);
			for (auto& a : e)
			{
				auto b = (EntityPhysicsProp*)a;

				if (b->body->getWorldTransform().getOrigin() == xyz)
				{
					std::cout << "Hit: " << b->modelName << std::endl;
					grabOffset = Util::vec3Conv(r.m_hitPointWorld) - Util::vec3Conv(r.m_collisionObject->getWorldTransform().getOrigin());
					grabRotation = r.m_collisionObject->getWorldTransform().getRotation();
					float q;
					grabRotation.getEulerZYX(grabWorldYaw, q, q);
					selectedEntity = b;
					distanceToSelected = glm::distance(Util::vec3Conv(r.m_hitPointWorld), engine->camera->pos);
					foundGoodObject = 1;
				}
			}
			if (!foundGoodObject)
				selectedEntity = nullptr;
		}
		else
		{
			selectedEntity = nullptr;
		}
	}

	if (engine->input->mouseDown(GLFW_MOUSE_BUTTON_LEFT) && selectedEntity != nullptr)
	{
		distanceToSelected += engine->input->scrollOffsetY * 2.0f;

		EntityPhysicsProp* ent = (EntityPhysicsProp*)selectedEntity;
		glm::vec3 vel = glm::normalize(engine->camera->pos - Util::vec3Conv(ent->body->getWorldTransform().getOrigin())) * ent->body->getMass() * 150.0f;
		ent->body->activate();

		btTransform nt;
		nt.setIdentity();
		nt.setRotation(btQuaternion(glm::radians(engine->camera->ang.z + grabWorldYaw), 0, 0));
		
		nt.setOrigin(Util::vec3Conv(engine->camera->pos + (engine->camera->lookVec * distanceToSelected) - grabOffset));
		ent->body->setWorldTransform(nt);
		ent->body->setLinearVelocity(btVector3(0, 0, 0));
		ent->body->setAngularVelocity(btVector3(0, 0, 0));
	}
}

void Editor::render()
{
}