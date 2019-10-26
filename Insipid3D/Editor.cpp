#include "Editor.h"
#include "engine.h"
#include "Entity.h"
#include "EntityPhysicsProp.h"
#include "Util.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/normal.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "GL/glew.h"
#include <GLFW/glfw3.h>

#include "Raytrace.h"

Editor::Editor(Engine *enginePtr)
{
	distanceToSelected = 10;
	engine = enginePtr;
	inEditor = 1;
}

void Editor::tick()
{
	if (engine->input->keyPressed(GLFW_KEY_TAB))
		inEditor = !inEditor;

	if (inEditor)
	{
		if (engine->input->mousePressed(GLFW_MOUSE_BUTTON_LEFT))
		{
			//Find physics object to select
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
			// Find point entity to select

		}

		if (engine->input->mouseDown(GLFW_MOUSE_BUTTON_LEFT))
		{
			if (selectedEntity != nullptr)
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
	}
}

void Editor::render()
{
	auto world = engine->getMap()->collisionState->world;
	btVector3 start = Util::vec3Conv(engine->camera->pos);
	btVector3 end = Util::vec3Conv(engine->camera->pos + engine->camera->lookVec * 1000.0f);

	btDynamicsWorld::ClosestRayResultCallback r(start, end);
	world->rayTest(start, end, r);



	GLuint flatShader = engine->shaderManager->getShader("shaders/flat");
	glUseProgram(flatShader);
	glActiveTexture(GL_TEXTURE0);
	glUniformMatrix4fv(glGetUniformLocation(flatShader, "model"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.)));
	glUniformMatrix4fv(glGetUniformLocation(flatShader, "view"), 1, GL_FALSE, glm::value_ptr(engine->camera->getViewMatrix()));
	glUniformMatrix4fv(glGetUniformLocation(flatShader, "proj"), 1, GL_FALSE, glm::value_ptr(engine->camera->getProjectionMatrix()));
	glUniform3f(glGetUniformLocation(flatShader, "col"), 1., 1., 1.);
	glDepthMask(GL_FALSE);

	glPointSize(4);
	glLineWidth(4);

	if (inEditor)
	{
		if (r.hasHit())
		{
			glm::vec3 p = Util::vec3Conv(r.m_hitPointWorld);
			glm::vec3 n = Util::vec3Conv(r.m_hitNormalWorld);

			glBegin(GL_LINES);
			glVertex3fv(glm::value_ptr(p));
			glVertex3fv(glm::value_ptr(p+n));
			glEnd();
		}
	}

	std::ostringstream dbg;
	dbg << "fps: " << floor(engine->averageFps) << " (" << engine->camera->pos.x << ", " << engine->camera->pos.y << ", " << engine->camera->pos.z << ")";
	dbg << " " << engine->camera->ang.x << ", " << engine->camera->ang.z;

	if (!engine->console->consoleShowing)
	{
		engine->drawDebugText(engine->fontManager->getFont("fonts/Roboto_Mono/RobotoMono-Regular.ttf", 18), 2, 720 - 18, dbg.str(), glm::vec3(0.f));
		engine->drawDebugText(engine->fontManager->getFont("fonts/Roboto_Mono/RobotoMono-Regular.ttf", 18), 3, 720 - 19, dbg.str());
	}

	glDepthMask(GL_TRUE);
}