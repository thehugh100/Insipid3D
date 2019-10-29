#include "Editor.h"
#include "engine.h"
#include "Entity.h"
#include "EntityPhysicsProp.h"
#include "EntityPoint.h"
#include "Util.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/normal.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/component_wise.hpp>

#include "GL/glew.h"
#include <GLFW/glfw3.h>

Editor::Editor(Engine *enginePtr)
{
	distanceToSelected = 10;
	engine = enginePtr;
	inEditor = 1;
	grabPos = glm::vec3(0.f);
	grabPosOld = glm::vec3(0.f);
	grabVel = glm::vec3(0.f);
}

void Editor::tick()
{
	if (engine->input->keyPressed(GLFW_KEY_TAB) && !engine->console->consoleShowing)
		inEditor = !inEditor;

	if (inEditor)
	{
		if (engine->input->keyPressed(GLFW_KEY_L) && !engine->console->consoleShowing)
		{
			auto world = engine->getMap()->collisionState->world;

			btVector3 start = Util::vec3Conv(engine->camera->pos);
			btVector3 end = Util::vec3Conv(engine->camera->pos + engine->camera->lookVec * 200.0f);

			btDynamicsWorld::ClosestRayResultCallback r(start, end);
			world->rayTest(start, end, r);

			if (r.hasHit())
			{
				engine->getMap()->addLight(new Light(Util::vec3Conv(r.m_hitPointWorld + r.m_hitNormalWorld), glm::vec3(1.), 200.f));
			}
		}

		if (engine->input->mousePressed(GLFW_MOUSE_BUTTON_LEFT))
		{
			//Find physics object to select
			auto world = engine->getMap()->collisionState->world;

			btVector3 start = Util::vec3Conv(engine->camera->pos);
			btVector3 end = Util::vec3Conv(engine->camera->pos + engine->camera->lookVec * 200.0f);

			btDynamicsWorld::ClosestRayResultCallback r(start, end);
			world->rayTest(start, end, r);
			selectedEntityLast = selectedEntity;
			bool foundGoodObject = 0;
			if (r.hasHit())
			{
				btVector3 xyz = r.m_collisionObject->getWorldTransform().getOrigin();

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
			pointSelectionLast = pointSelection;
			pointSelection = nullptr;
			pointSelectionAxis = glm::vec3(0);
			EntityList l;
			pointSelectionDistance = 0;
			engine->entityManger->getEntityByTraits("Point", &l);
			
			for (auto& i : l)
			{
				EntityPoint* e = (EntityPoint*)i;
				glm::vec2 screenPos;
				if (engine->camera->worldToScreen(e->getPos(), screenPos))
				{
					if (r.hasHit() && glm::distance(engine->camera->pos, Util::vec3Conv(r.m_hitPointWorld)) <
						glm::distance(engine->camera->pos, e->getPos()))
						continue;

					if (glm::distance(engine->screen * .5f, screenPos) < 64.f)
					{
						pointSelection = e;
						pointSelectionDistance = glm::distance(engine->camera->pos, e->getPos());

						glm::vec3 c1 = glm::cross(engine->camera->lookVec, glm::vec3(1, 0, 0));
						float d1 = glm::dot(e->getPos() - engine->camera->pos, c1) / glm::length(c1);

						glm::vec3 c2 = glm::cross(engine->camera->lookVec, glm::vec3(0, 1, 0));
						float d2 = glm::dot(e->getPos() - engine->camera->pos, c2) / glm::length(c2);

						glm::vec3 c3 = glm::cross(engine->camera->lookVec, glm::vec3(0, 0, 1));
						float d3 = glm::dot(e->getPos() - engine->camera->pos, c3) / glm::length(c3);

						glm::vec3 a = glm::abs(glm::vec3(d1, d2, d3));
						a /= glm::compMin(a);
						if (a.x == 1)
							pointSelectionAxis = glm::vec3(1, 0, 0);
						if (a.y == 1)
							pointSelectionAxis = glm::vec3(0, 1, 0);
						if (a.z == 1)
							pointSelectionAxis = glm::vec3(0, 0, 1);

						std::cout << "Selected Axis: " << Util::printVec3(pointSelectionAxis) << std::endl;
						break;
					}
				}
			}
			if (selectedEntity != nullptr && foundGoodObject)
				pointSelection = nullptr;
		}

		if (engine->input->mouseDown(GLFW_MOUSE_BUTTON_LEFT))
		{
			if (selectedEntity != nullptr && pointSelection == nullptr)
			{
				distanceToSelected += engine->input->scrollOffsetY * 2.0f;

				EntityPhysicsProp* ent = (EntityPhysicsProp*)selectedEntity;
				glm::vec3 vel = glm::normalize(engine->camera->pos - Util::vec3Conv(ent->body->getWorldTransform().getOrigin())) * ent->body->getMass() * 150.0f;
				ent->body->activate();

				btTransform nt;
				nt.setIdentity();
				nt.setRotation(btQuaternion(glm::radians(engine->camera->ang.z + grabWorldYaw), 0, 0));
				grabPosOld = grabPos;
				grabPos = engine->camera->pos + (engine->camera->lookVec * distanceToSelected) - grabOffset;
				
				nt.setOrigin(Util::vec3Conv(grabPos));
				
				ent->body->setWorldTransform(nt);
				ent->body->setLinearVelocity(btVector3(0, 0, 0));
				ent->body->setAngularVelocity(btVector3(0, 0, 0));

				grabVel = (grabPos - grabPosOld);

				ent->body->applyCentralImpulse(Util::vec3Conv(grabVel * 
					engine->variables->getVarOrCreate("editorGrabVelocityMultiplier", "500", Variable::valFloat)->getFloat()));

				if (engine->input->keyPressed(GLFW_KEY_DELETE))
				{
					ent->destroy();
					selectedEntity = nullptr;
				}
			}

			if (pointSelection != nullptr && pointSelectionAxis != glm::vec3(0) && pointSelection == pointSelectionLast)
			{
				EntityPoint* p = (EntityPoint*)pointSelection;
				
				glm::vec3 newPos = (p->getPos() * (1.0f - pointSelectionAxis)) + 
					(engine->camera->pos + engine->camera->lookVec * glm::distance(p->getPos(), engine->camera->pos)) * pointSelectionAxis;
				p->setPos(newPos);
			}
		}
		//grabVel = glm::mix(grabVel, glm::vec3(0.f), 1.f - glm::pow(.9f, engine->deltaTime));
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

		if ((pointSelection != nullptr || selectedEntity != nullptr) && engine->variables->getVarOrCreate("drawEntityDebugInfo", "0", Variable::valInt)->getInt())
		{
			int canSee = 0;
			glm::vec2 pos;
			Entity* e = nullptr;

			if (pointSelection != nullptr)
			{
				EntityPoint* p = (EntityPoint*)pointSelection;
				e = p;
				canSee = engine->camera->worldToScreen(p->getPos(), pos);
			}
			if (selectedEntity != nullptr)
			{
				EntityPhysicsProp* p = (EntityPhysicsProp*)selectedEntity;
				e = p;
				canSee = engine->camera->worldToScreen(Util::vec3Conv(p->body->getWorldTransform().getOrigin()), pos);
			}

			if (canSee)
			{
				int entId = 0;
				EntityList l;
				engine->entityManger->getAllEntities(&l);
				for (auto& i : l)
				{
					if (i == pointSelection || i == selectedEntity)
						break;
					entId++;
				}
				engine->drawDebugText(engine->fontManager->getFont("fonts/Roboto_Mono/RobotoMono-Regular.ttf", 18)
					, pos.x, pos.y, "ID :" + std::to_string(entId));
				pos.y += 18;
				for (auto& i : e->vars.vals)
				{
					engine->drawDebugText(engine->fontManager->getFont("fonts/Roboto_Mono/RobotoMono-Regular.ttf", 18)
						, pos.x, pos.y, i.first + ": " + i.second.toString());
					pos.y += 18;
				}
				//engine->drawDebugText(engine->fontManager->getFont("fonts/Roboto_Mono/RobotoMono-Regular.ttf", 18), pos.x-1, pos.y-1, "Light", glm::vec3(0.f));
				//engine->drawDebugText(engine->fontManager->getFont("fonts/Roboto_Mono/RobotoMono-Regular.ttf", 18), pos.x, pos.y, "Light");
			}
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