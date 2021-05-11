#include "EntityDebugCrate.h"
#include "engine.h"
#include "EntityManager.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

#include "Raytrace.h"

#include "BulletCollision/CollisionShapes/btShapeHull.h"

#include "Util.h"

EntityDebugCrate::EntityDebugCrate(glm::vec3 origin)
	:origin(origin)
{
	entityTraits.setTrait("EntityDebugCrate");
	entityTraits.setTrait("PhysicsEntity");

	std::cout << "Created debug crate" << std::endl;
}

void EntityDebugCrate::tick()
{
	/*EntityList e;
	if (engine->entityManger->getEntityByTraits("EntityDebugCrate", &e))
	{
		for (auto& i : e)
		{
			if (i == this)
				continue;
			EntityDebugCrate* crate = (EntityDebugCrate*)i;
			vel += glm::normalize(crate->pos - pos) * 0.001f;
		}
	}
	pos += vel;
	*/
}

void EntityDebugCrate::init()
{
	model = engine->meshManager->getMesh("models/crate.glb");

	btConvexHullShape convexHullShape((btScalar *)model->meshEntries[0]->meshRef->mVertices, model->meshEntries[0]->meshRef->mNumVertices, 3 * sizeof(float));
	convexHullShape.setMargin(0);
	btShapeHull* hull = new btShapeHull(&convexHullShape);
	hull->buildHull(0);
	btConvexHullShape* pConvexHullShape = new btConvexHullShape((const btScalar*)hull->getVertexPointer(), hull->numVertices(), sizeof(btVector3));

	btTransform t;
	t.setIdentity();
	t.setOrigin(btVector3(origin.x, origin.y, origin.z));
	btVector3 inertia;
	pConvexHullShape->calculateLocalInertia(4, inertia);
	btMotionState* motion = new btDefaultMotionState(t);
	btRigidBody::btRigidBodyConstructionInfo info(4, motion, pConvexHullShape, inertia);
	body = new btRigidBody(info);
	engine->getMap()->collisionState->world->addRigidBody(body);

	delete hull;
	initialised = 1;
}

void EntityDebugCrate::render()
{
	GLuint shader = engine->shaderManager->getShader("shaders/default");

	btTransform t;
	body->getMotionState()->getWorldTransform(t);
	float mat[16];
	t.getOpenGLMatrix(mat);

	float brightnessMultiplier = !RayTrace::rayTrace(glm::vec3(mat[12], mat[13], mat[14]), -engine->getMap()->sunVec, engine->getMap()->getMesh()).hit;

	glUseProgram(shader);
	glActiveTexture(GL_TEXTURE0);
	glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, mat);
	glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, glm::value_ptr(engine->camera->getViewMatrix()));
	glUniformMatrix4fv(glGetUniformLocation(shader, "proj"), 1, GL_FALSE, glm::value_ptr(engine->camera->getProjectionMatrix()));
	glUniform3fv(glGetUniformLocation(shader, "cameraPos"), 1, glm::value_ptr(engine->camera->pos));
	glUniform4fv(glGetUniformLocation(shader, "sunVec"), 1, glm::value_ptr( glm::vec4(engine->getMap()->sunVec, brightnessMultiplier)));
	
	int numLights = engine->getMap()->lights.size();

	glUniform1i(glGetUniformLocation(shader, "numLights"), numLights);

	for (int i = 0; i < numLights; i++)
	{
		std::string lightID = "lights[" + std::to_string(i) + "]";

		float vis = 1;

		btVector3 start = body->getWorldTransform().getOrigin();
		btVector3 end = Util::vec3Conv(engine->getMap()->lights[i]->pos);

		btCollisionWorld::AllHitsRayResultCallback r(start, end);
		engine->getMap()->collisionState->world->rayTest(start, end, r);

		if (r.hasHit())
		{
			for (int q = 0; q < r.m_collisionObjects.size(); q++)
			{
				//std::cout << r.m_collisionObjects[q]->getCollisionShape()->getName() << std::endl;
				vis = 0;
			}
		}

		glUniform3fv(glGetUniformLocation(shader, std::string(lightID + ".pos").c_str()), 1, glm::value_ptr(engine->getMap()->lights[i]->pos));
		glUniform3fv(glGetUniformLocation(shader, std::string(lightID + ".col").c_str()), 1, glm::value_ptr(engine->getMap()->lights[i]->col));
		glUniform3fv(glGetUniformLocation(shader, std::string(lightID + ".dir").c_str()), 1, glm::value_ptr(engine->getMap()->lights[i]->dir));
		glUniform1f(glGetUniformLocation(shader, std::string(lightID + ".intensity").c_str()), engine->getMap()->lights[i]->intensity * vis);
	}

	model->render();
}
