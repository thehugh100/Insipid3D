#pragma once
#include "entity.h"
#include "Mesh.h"
#include <glm/glm.hpp>
#include "EntityPhysicsProp.h"
#include <btBulletDynamicsCommon.h>
#include "PID.h"

struct EntityLight;
struct EntityFlare;

struct EntityMissile : EntityPhysicsProp
{
	EntityMissile(glm::vec3 origin);
	EntityMissile();

	virtual void tick();
	virtual void destroy();
	virtual void render();

	glm::vec3 oldVelocity;
	glm::vec3 velocity;
	int triggered;
	float explosionTime;
	bool set;
	int stage;
	float nextStage;
	float launchedTime;
	EntityLight* cLight;
	EntityFlare* flare;

	float yawCorrection;
	float thrustAmount;

	glm::vec3 targetPos;
	bool hasTarget;

	PIDController<double>* pid;
	PIDController<double>* pidYaw;
	PIDController<double>* thrustPid;
};
