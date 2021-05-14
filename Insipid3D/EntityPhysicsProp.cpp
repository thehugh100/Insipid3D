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
#include "EntityLight.h"

#include <glm/gtc/quaternion.hpp>
#include <glm/common.hpp>
#include "Base64.h"
#include "MeshManager.h"

EntityPhysicsProp::EntityPhysicsProp(std::string modelName, glm::vec3 origin, float mass)
	:origin(origin), modelName(modelName), mass(mass)
{
	entityType = "EntityPhysicsProp";
	entityTraits.setTrait("EntityPhysicsProp");
	entityTraits.setTrait("PhysicsEntity");
	active = 1;
	backfaceCull = 1;
	transform = glm::mat4(0.f);
	physicsState = "";
	vars.registerVal("origin", Serializer(&origin));
	vars.registerVal("modelName", Serializer(&modelName));
	vars.registerVal("transform", Serializer(&transform));
	vars.registerVal("mass", Serializer(&mass));
	vars.registerVal("physicsState", Serializer(&physicsState));
}

EntityPhysicsProp::EntityPhysicsProp()
{
	entityType = "EntityPhysicsProp";
	entityTraits.setTrait("EntityPhysicsProp");
	entityTraits.setTrait("PhysicsEntity");
	mass = 1;
	body = nullptr;
	origin = glm::vec3(0.);
	modelName = "";
	model = nullptr;
	active = 1;
	backfaceCull = 1;
	transform = glm::mat4(0.f);
	physicsState = "";
	vars.registerVal("origin", Serializer(&origin));
	vars.registerVal("modelName", Serializer(&modelName));
	vars.registerVal("transform", Serializer(&transform));
	vars.registerVal("mass", Serializer(&mass));
	vars.registerVal("physicsState", Serializer(&physicsState));
}

EntityPhysicsProp::~EntityPhysicsProp()
{
	std::cout << "removed EntityPhysicsProp" << std::endl;
	remove();
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
	pConvexHullShape->calculateLocalInertia(mass, inertia);
	btMotionState* motion = new btDefaultMotionState(t);
	btRigidBody::btRigidBodyConstructionInfo info(mass, motion, pConvexHullShape, inertia);
	body = new btRigidBody(info);
	engine->getMap()->collisionState->world->addRigidBody(body);

	delete hull;
	initialised = 1;
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

nlohmann::json EntityPhysicsProp::serialize()
{
	btDefaultSerializer* serializer = new btDefaultSerializer();

	size_t sBufSize = body->calculateSerializeBufferSize();
	char* sBuf = new char[sBufSize];

	body->serialize(sBuf, serializer);

	physicsState = macaron::Base64::Encode(std::string((const char*)sBuf, sBufSize));

	delete serializer;
	delete sBuf;

	return nlohmann::json::parse(vars.serialize());
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

		engine->getMap()->getLightsShaderUniforms(shader, getPosition());

		model->render();

		if (!backfaceCull)
			glEnable(GL_CULL_FACE);


		//GLuint flatShader = engine->shaderManager->getShader("shaders/flat");
		//glUseProgram(flatShader);
		//glActiveTexture(GL_TEXTURE0);
		//glUniformMatrix4fv(glGetUniformLocation(flatShader, "model"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.)));
		//glUniformMatrix4fv(glGetUniformLocation(flatShader, "view"), 1, GL_FALSE, glm::value_ptr(engine->camera->getViewMatrix()));
		//glUniformMatrix4fv(glGetUniformLocation(flatShader, "proj"), 1, GL_FALSE, glm::value_ptr(engine->camera->getProjectionMatrix()));
		//glUniform3f(glGetUniformLocation(flatShader, "col"), 1., 1., 1.);
		//glDepthMask(GL_FALSE);

		//glPointSize(4);
		//glLineWidth(4);

		//auto rot = body->getWorldTransform().getRotation();
		//glm::quat gQuat = glm::quat(rot.getW(), rot.getX(), rot.getY(), rot.getZ());

		//glBegin(GL_LINES);
		//glVertex3fv(glm::value_ptr( getPosition() ));
		//glVertex3fv(glm::value_ptr( getPosition() + gQuat * glm::vec3(0, 1, 0) ));
		//glEnd();
		//

		//glDepthMask(GL_TRUE);
	}
}
