#pragma once
#include "entity.h"
#include "Mesh.h"
#include <glm/glm.hpp>

#include <btBulletDynamicsCommon.h>

struct EntityPhysicsProp : Entity
{
	EntityPhysicsProp(std::string modelName, glm::vec3 origin, float mass);
	EntityPhysicsProp();

	~EntityPhysicsProp();

	virtual void tick();
	virtual void render();
	virtual void init();
	virtual void destroy();
	virtual void remove();
	virtual void applyForce(glm::vec3 force);
	virtual void applyImpulse(glm::vec3 impulse);
	virtual void update();
	virtual void setTransform(glm::mat4 transform);
	virtual nlohmann::json serialize();
	
	std::string physicsState;
	glm::vec3 getPosition();
	glm::vec3 origin;
	float mass;
	std::string modelName;

	Mesh* model;
	btRigidBody* body;
	glm::mat4 transform;

	bool backfaceCull;
};