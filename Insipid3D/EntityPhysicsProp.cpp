#include "EntityPhysicsProp.h"
#include "engine.h"
#include "EntityManager.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include "Raytrace.h"
#include "BulletCollision/CollisionShapes/btShapeHull.h"
#include "Util.h"

EntityPhysicsProp::EntityPhysicsProp(std::string modelName, glm::vec3 origin)
	:origin(origin), modelName(modelName)
{
	entityType = "EntityPhysicsProp";
	entityTraits.setTrait("EntityPhysicsProp");
	entityTraits.setTrait("PhysicsEntity");
	active = 1;
	backfaceCull = 1;
	transform = glm::mat4(0.f);
	vars.registerVal("origin", Serializer(&origin));
	vars.registerVal("modelName", Serializer(&modelName));
	vars.registerVal("transform", Serializer(&transform));
}

EntityPhysicsProp::EntityPhysicsProp()
{
	entityType = "EntityPhysicsProp";
	entityTraits.setTrait("EntityPhysicsProp");
	entityTraits.setTrait("PhysicsEntity");

	body = nullptr;
	origin = glm::vec3(0.);
	modelName = "";
	model = nullptr;
	active = 1;
	backfaceCull = 1;
	transform = glm::mat4(0.f);
	vars.registerVal("origin", Serializer(&origin));
	vars.registerVal("modelName", Serializer(&modelName));
	vars.registerVal("transform", Serializer(&transform));
}

void EntityPhysicsProp::tick()
{
}

void EntityPhysicsProp::init()
{
	model = engine->meshManager->getMesh(modelName);

	btConvexHullShape convexHullShape((btScalar*)model->meshEntries[0]->meshRef->mVertices, model->meshEntries[0]->meshRef->mNumVertices, 3 * sizeof(float));
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
}

void EntityPhysicsProp::destroy()
{
	remove();
}

void EntityPhysicsProp::remove()
{
	engine->getMap()->collisionState->world->removeRigidBody(body);
	active = 0;
}

void EntityPhysicsProp::applyForce(glm::vec3 force)
{
	body->activate();
	body->applyCentralImpulse(Util::vec3Conv(force));
}

void EntityPhysicsProp::applyImpulse(glm::vec3 impulse)
{
	body->activate();
	body->applyCentralImpulse(Util::vec3Conv(impulse));
}

void EntityPhysicsProp::update()
{
	float m[16];
	body->getWorldTransform().getOpenGLMatrix(m);
	transform = glm::make_mat4(m);
}

void EntityPhysicsProp::setTransform(glm::mat4 transform)
{
	body->getWorldTransform().setFromOpenGLMatrix(glm::value_ptr(transform));
}

glm::vec3 EntityPhysicsProp::getPosition()
{
	btTransform t;
	body->getMotionState()->getWorldTransform(t);
	return Util::vec3Conv(t.getOrigin());
}

void EntityPhysicsProp::render()
{
	if (active)
	{
		update();
		if (!backfaceCull)
			glDisable(GL_CULL_FACE);

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
		glUniform4fv(glGetUniformLocation(shader, "sunVec"), 1, glm::value_ptr(glm::vec4(engine->getMap()->sunVec, brightnessMultiplier)));

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

		if (!backfaceCull)
			glEnable(GL_CULL_FACE);
	}
}
