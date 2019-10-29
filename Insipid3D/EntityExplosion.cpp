#include "EntityExplosion.h"
#include "engine.h"
#include "Util.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

EntityExplosion::EntityExplosion(glm::vec3 origin_, float magnitude)
	:magnitude(magnitude)
{
	entityType = "EntityExplosion";
	entityTraits.setTrait("EntityExplosion");
	origin = origin_;
	point = &origin;
	explosionTime = 200 + Util::randFloat() * 100.;
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

		if (engine->getElapsedTimeMS() > clearTime)
			remove();
	}
}

void EntityExplosion::render()
{
	if (active)
	{
		GLuint shader = engine->shaderManager->getShader("shaders/explosion");
		//glDisable(GL_CULL_FACE);
		glUseProgram(shader);
		glActiveTexture(GL_TEXTURE0);
		glm::mat4 model = glm::rotate(glm::translate(glm::mat4(1.f), *point), 180.f, rotation);

		glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, glm::value_ptr(engine->camera->getViewMatrix()));
		glUniformMatrix4fv(glGetUniformLocation(shader, "proj"), 1, GL_FALSE, glm::value_ptr(engine->camera->getProjectionMatrix()));

		glUniform1f(glGetUniformLocation(shader, "scale"), (clearTime - engine->getElapsedTimeMS()) / explosionTime);

		auto m = engine->meshManager->getMesh("models/editor/sphere.glb");

		glBindVertexArray(m->meshEntries[0]->vao);
		glDrawElements(GL_TRIANGLES, m->meshEntries[0]->elementCount, GL_UNSIGNED_INT, (GLvoid*)0);
		glBindVertexArray(0);
		//glEnable(GL_CULL_FACE);
	}
}

void EntityExplosion::init()
{
	noiseTex = engine->textureManager->loadTexture("textures/noise.png");
	clearTime = engine->getElapsedTimeMS() + explosionTime;
	rotation = Util::randVec();
}

void EntityExplosion::remove()
{
	active = 0;
}
